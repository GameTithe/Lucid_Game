// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MPP/Weapon/Weapon.h"
#include "MPP/ShooterComponent/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SAnimInstance.h"  
#include "MPP/MPP.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "MPP/GameMode/SGameMode.h"
#include "TimerManager.h"
#include "MPP/PlayerState/SPlayerState.h"
#include "MPP/Weapon/WeaponTypes.h"
#include "Kismet/GameplayStatics.h"
#include "MPP/ShooterComponent/BuffComponent.h"

ASCharacter::ASCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheaDWidget"));
	OverheadWidget->SetupAttachment(GetMesh());

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->RotationRate = FRotator(0, 0, 800.0f);

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline")); 
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ASCharacter, Health);
	DOREPLIFETIME(ASCharacter, bDisableGameplay);
}
void ASCharacter::SpawnDefaultWeapon()
{
	SGameMode = SGameMode == nullptr ? GetWorld()->GetAuthGameMode<ASGameMode>() : SGameMode;

	UWorld* World = GetWorld();
	if (SGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		Combat->EquipWeapon(StartingWeapon);
	}
}


void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultWeapon();
	UpdateHUDHealth();
	UpdateHUDAmmo();


	// 서버 데미지 처리 
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceivedDamage); 
	}
}
void ASCharacter::UpdateHUDHealth()
{  
	SController = SController == nullptr ? Cast<ASPlayerController>(Controller) : SController;
	if (SController)
	{ 
		SController->SetHUDHealth(Health, MaxHealth);
	} 
}
void ASCharacter::UpdateHUDAmmo()
{
	SController = SController == nullptr ? Cast<ASPlayerController>(Controller) : SController;

	if (SController && Combat && Combat->EquippedWeapon)
	{
		SController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		SController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 

	AimOffset(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

void ASCharacter::PollInit()
{
	if (SPlayerState == nullptr)
	{
		SPlayerState = GetPlayerState<ASPlayerState>();
		if (SPlayerState)
		{
			SPlayerState->AddToScore(0.0f);
			SPlayerState->AddToDefeats(0);
		}
	}
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetinitSpeed(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched, Combat->AimWalkSpeed);
	}
}

void ASCharacter::Destroyed()
{
	Super::Destroyed();

	SGameMode = SGameMode == nullptr ? GetWorld()->GetAuthGameMode<ASGameMode>() : SGameMode; 
	bool bMatchNotInProgress = SGameMode && SGameMode->GetMatchState() != MatchState::InProgress;

	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy(); 
	}
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ThisClass::LookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &ThisClass::EquipButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), EInputEvent::IE_Released, this, &ThisClass::AimButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ThisClass::FireButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &ThisClass::ReloadButtonPressed);

	PlayerInputComponent->BindAction(TEXT("Chat"), EInputEvent::IE_Pressed, this, &ThisClass::ChatButtonPressed);
}

void ASCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void ASCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ASCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ASCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASCharacter::Jump()
{
	if (bDisableGameplay) return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}


void ASCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		if (HasAuthority())
		{ 
			if (OverlappingWeapon)
			{
				Combat->EquipWeapon(OverlappingWeapon);
			}
			else if (Combat->ShoulSwapWeapons())
			{
				Combat->SwapWeapons();
			}
		}
		else
		{ 
			// only send to server
			ServerEquipButtonPressed();
		}

		Combat->bCanFire = true;
	}
}
void ASCharacter::ServerEquipButtonPressed_Implementation()
{   
	if (Combat)
	{   
		if(OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if(Combat->ShoulSwapWeapons())
		{
			Combat->SwapWeapons();
		}
	}
}
 

void ASCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();	
	}
}

void ASCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ASCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ASCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;
	float Speed = Velocity.Size();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.0f && !bIsInAir) // stading, not jumping
	{
		FRotator CurrentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.0f || bIsInAir) // running or jumping
	{
		StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		AO_Yaw = 0.0f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		FVector2D InRange(270.0f, 360.0f);
		FVector2D OutRange(-90.0f, 0.0f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ASCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ASCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ASCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->Reload(); 
	}
}

void ASCharacter::ChatButtonPressed()
{
	SController = SController == nullptr ? Cast<ASPlayerController>(Controller) : SController;
	if (SController)
	{ 
		SController->ActiveChatBox();
	}
}


void ASCharacter::Elim(bool bPlayerLeftGame)
{
	DropOrDestroyWeapons();
	MulticastElim(bPlayerLeftGame); 

}
void ASCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;

	bElimmed = true;
	PlayElimMontage();

	//start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);

		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), -0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.0f);
	}

	StartDissolve();


	//disable character movement
	GetCharacterMovement()->DisableMovement(); //stop move
	GetCharacterMovement()->StopMovementImmediately(); //stop rotate
	bDisableGameplay = true;

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool bHideSniperScope =
		Combat && Combat->EquippedWeapon &&
		Combat->bAiming &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;


	if (bHideSniperScope) ShowSniperScopeWidget(false);
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ASCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void ASCharacter::ElimTimerFinished()
{
	SGameMode = SGameMode == nullptr ? GetWorld()->GetAuthGameMode<ASGameMode>() : SGameMode;

	if (SGameMode && !bLeftGame)
	{
		SGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}

void ASCharacter::DropOrDestroyWeapons()
{
	if (SController)
	{
		SController->SetHUDWeaponAmmo(0);
	}

	if (Combat && Combat->EquippedWeapon)
	{
		if (Combat->EquippedWeapon->bDestroyWeapon)
		{
			Combat->EquippedWeapon->Destroy();
		}
		else
		{
			Combat->EquippedWeapon->Dropped();
		}
	}
}

void ASCharacter::ServerLeaveGame_Implementation()
{
	SGameMode = SGameMode == nullptr ? GetWorld()->GetAuthGameMode<ASGameMode>() : SGameMode;

	SPlayerState = SPlayerState == nullptr ? GetPlayerState<ASPlayerState>() : SPlayerState;

	if (SGameMode && SPlayerState)
	{
		SGameMode->PlayerLeftGame(SPlayerState);
	}
}

//For Server
void ASCharacter::ReceivedDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	SGameMode = SGameMode == nullptr ? GetWorld()->GetAuthGameMode<ASGameMode>() : SGameMode;
	if (bElimmed || SGameMode == nullptr) return;



	Damage = SGameMode->CalculateDamage(InstigatedBy, Controller, Damage);

	
	Health = FMath::Clamp(Health - Damage, 0.0, MaxHealth);
	PlayHitReactMontage();
	UpdateHUDHealth();

	if (Health <= 0.0f)
	{
		if (SGameMode)
		{
			SController = SController == nullptr ? Cast<ASPlayerController>(Controller) : SController;
			ASPlayerController* Instigate = Cast<ASPlayerController>(InstigatedBy);
			SGameMode->PlayerEliminated(this, SController, Instigate);
		}
	}
}

void ASCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);

		FName SectionName;
		SectionName = bAiming ? "RifleHip" : "RifleAim";
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ASCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);

		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLancher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_ShotGun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ASCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);

		FName SectionName;
		SectionName = FName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ASCharacter::PlayElimMontage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ElimReactMontage)
	{
		AnimInstance->Montage_Play(ElimReactMontage); 
	}
} 

FVector ASCharacter::GetHitTarget()
{
	if (Combat == nullptr)return FVector();

	return Combat->HitTarget;

}


void ASCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.0f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
}

void ASCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;

	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold )
	{
		GetMesh()->SetVisibility(false);
		
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
			GetEquippedWeapon()->GetWeaponMesh()->bOwnerNoSee = true;
	}
	else
	{
		GetMesh()->SetVisibility(true);
		
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
			GetEquippedWeapon()->GetWeaponMesh()->bOwnerNoSee = false;
	}
} 

void ASCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void ASCharacter::UpdateDissloveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue); 
	}
}

void ASCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ASCharacter::UpdateDissloveMaterial);
	
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ASCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	//server는 replicate가 안되니 여기서 처리해준다. 
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}
bool ASCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ASCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ASCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;

	return Combat->EquippedWeapon;
}

ECombatState ASCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_DefaultMAX;

	return Combat->CombatState;

}

void ASCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}
