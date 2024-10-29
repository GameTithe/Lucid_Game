// Fill out your copyright notice in the Description page of Project Settings.
#include "CombatComponent.h"
#include "MPP/Weapon/Weapon.h"
#include "MPP/Character/SCharacter.h"
#include "Engine/SkeletalMeshSocket.h" 
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" 
#include "MPP/PlayerController/SPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"
#include "MPP/Character/SAnimInstance.h"
#include "MPP/ShooterComponent/BuffComponent.h"

//#include "MPP/Interface/CrosshairInterface.h"

#define TRACE_LENGTH 80000.0f

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bCanFire = true;

	BaseWalkSpeed = 600.0f;
	AimWalkSpeed = 450.0f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	 
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurFOV = DefaultFOV;
		}
		if (Character->HasAuthority())
		{
			InitializedCarriedAmmo();
		}
	} 
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHUDCrosshair(DeltaTime);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrossHair(HitResult);
		HitTarget = HitResult.ImpactPoint;
		
		if (Character->GetFollowCamera())
		{
			InterpFOV(DeltaTime);
		}
	}


}
void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{ 
	if(Character == nullptr) return;
	 
	if (!Controller)
	{
		Controller = Cast<ASPlayerController>(Character->Controller);
	}
	if(Controller)
	{
		if (!HUD)
			HUD = Cast<ASHUD>(Controller->GetHUD());

		if(HUD)
		{ 
			
			if (EquippedWeapon)
			{ 
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairButtom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairButtom = nullptr;
			}

			float MaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
			FVector2D WalkSpeedRange(0.0f, MaxWalkSpeed);
			FVector2D MappingRange(0.0f, 1.0f);

			FVector CurVelocity = Character->GetCharacterMovement()->Velocity;
			CurVelocity.Z = 0.0f;

			CrossHairSpreadFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, MappingRange, CurVelocity.Size());
			if(Character->GetCharacterMovement()->IsFalling())
			{
				CrossHairJumpFactor = FMath::FInterpTo(CrossHairJumpFactor, 2.25f, DeltaTime, 2.25f);
			}
			else 
			{
				CrossHairJumpFactor = FMath::FInterpTo(CrossHairJumpFactor, 0.0f, DeltaTime, 30.0f);
			}
			
			if (Character->IsAiming())
			{
				CroosHairAimFactor = FMath::FInterpTo(CroosHairAimFactor, 0.5f, DeltaTime, 30.0f);
			
			}
			else
			{
				CroosHairAimFactor = FMath::FInterpTo(CroosHairAimFactor, 0.f, DeltaTime, 30.0f);
			}
			CroosHairShootFactor = FMath::FInterpTo(CroosHairShootFactor, 0.0f, DeltaTime, 2.0f);

			HUDPackage.CrosshairSpread =
				0.5f
				+ CrossHairSpreadFactor
				+ CrossHairJumpFactor
				+ CroosHairShootFactor
				-CroosHairAimFactor;

			HUD->SetCrosshair(HUDPackage);
		}
	} 
} 

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (Character && Character->GetFollowCamera())
	{   
		if (Character->IsAiming())
		{
			CurFOV = FMath::FInterpTo(CurFOV, EquippedWeapon->ZoomedFOV,DeltaTime, EquippedWeapon->ZoomedInterpSpeed);
		}
		else
		{
			CurFOV = FMath::FInterpTo(CurFOV, DefaultFOV, DeltaTime, EquippedWeapon->ZoomedInterpSpeed);
		}
		if (Character && Character->GetFollowCamera())
		{
			Character->GetFollowCamera()->SetFieldOfView(CurFOV);
		}
	}
}
 
void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed: BaseWalkSpeed;
	} 

	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonpressed = bPressed;

	if (bFireButtonpressed)
	{ 
		Fire();
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if (Character && Character->HasAuthority() && CarriedAmmo != 0)
	{
		UpdateShotgunAmmoValue();
	}
}

void UCombatComponent::Fire()
{  
	//clinet에 equippedWeapon이 안된다.
	//if (EquippedWeapon == nullptr) { 
	//	return;
	//} 
	 
	if (CanFire())
	{ 
		bCanFire = false;

		ServerFire(HitTarget);

		if (EquippedWeapon)
			CroosHairShootFactor = 2.0f;

		StartFireTimer();
	}

	if (EquippedWeapon)
	{ 
		if(EquippedWeapon && EquippedWeapon->IsEmpty())
			EmptyGunSound();
	}
}

void UCombatComponent::ServerFire_Implementation(FVector_NetQuantize TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(FVector_NetQuantize TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;

	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}

}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&ThisClass::FinishFireTimer,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FinishFireTimer()
{

	bCanFire = true;
	if (EquippedWeapon == nullptr) return;

	if (bFireButtonpressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	
	if(EquippedWeapon && EquippedWeapon->IsEmpty())
		EmptyGunSound();
}

void UCombatComponent::EmptyGunSound()
{

	if (EquippedWeapon && Character && EquippedWeapon->EmptyGunSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EmptyGunSound,
			Character->GetActorLocation()
		);
	}
}



bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr)  return false;  
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
} 

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{ 
	if (Character == nullptr || WeaponToEquip == nullptr) return;  
	 

	if (EquippedWeapon)
	{
		EquippedWeapon->bDestroyWeapon = false;
		EquippedWeapon->Dropped();
	}
	 
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->bDestroyWeapon = true;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped); 

	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EquipSound,
			Character->GetActorLocation()
		);

	}
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()]; 
	}

	Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;
	
	if (Controller)
	{ 
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	} 
	const USkeletalMeshSocket* HandSocket =  Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}


void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		if (EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				EquippedWeapon->EquipSound,
				Character->GetActorLocation()
			);

		}
		
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::InitializedCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLancher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, StartingSubmachineGun);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_ShotGun, StartingShotGun);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSniperfile);
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	bool bJumpToShotgunEnd =
		CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr&&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun &&
		CarriedAmmo == 0; 

	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}
}

// Server & clinent
void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatState::ECS_DefaultMAX)
		ServerReload();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;

	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		//int32 AmountCarriedAmmo = CarriedAmmo;
		int32 AmountCarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 ReloadAmmo = FMath::Min(RoomInMag, AmountCarriedAmmo);
		return FMath::Clamp(ReloadAmmo, 0, AmountCarriedAmmo);
	}

	else
		return 0;
}

void UCombatComponent::FinishedReload()
{
	if (Character == nullptr) return;
	 
	if (Character->HasAuthority())
	{ 
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateCarriedAmmo();
	}
	if (bFireButtonpressed)
	{
		Fire();
	}
}

//server & client
void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr) return;

	UpdateCarriedAmmo();  
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();

}
void UCombatComponent::UpdateCarriedAmmo()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun)
		UpdateShotgunAmmoValue();
	int32 ReloadAmount = AmountToReload(); 
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] += ReloadAmount;
		UE_LOG(LogTemp, Warning, TEXT("%d"), ReloadAmount);
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		//CarriedAmmo -= ReloadAmount;
	}

	//for server
	Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AmmoToAdd(ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValue()
{ 
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()]; 
	}
	Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AmmoToAdd(1);

	if (EquippedWeapon->IsFull() || CarriedAmmo == 0 )
	{
		JumpToShotgunEnd();
	}

}
void UCombatComponent::JumpToShotgunEnd()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->GetReloadMontage())
	{
		//AnimInstance->Montage_Play(Character->GetReloadMontage());
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
	}
}

void UCombatComponent::PickupAmmo(EWeaponType type, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(type))
	{
		CarriedAmmoMap[type] = FMath::Clamp(CarriedAmmoMap[type] + AmmoAmount, 0, MaxCarriedAmmo);

		UpdateCarriedAmmo();
	}
}


//client
void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonpressed)
		{
			Fire();
		}
		break;
	}
}


void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}
void UCombatComponent::TraceUnderCrossHair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float Distance = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (Distance + 100.0f);
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UCrosshairInterface>())
		{
			HUDPackage.LinearColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.LinearColor = FLinearColor::White;
		}
	}
}
