// Fill out your copyright notice in the Description page of Project Settings.


#include "SMonster.h" 
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "MPP/HUD/MonsterHUD.h"
#include "MPP/MPP.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h" 
#include "MPP/PlayerController/MonsterAIController.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MPP/Character/SCharacter.h" 

ASMonster::ASMonster()
{ 
	
	PrimaryActorTick.bCanEverTick = true; 

	MaxHealth = 100.0f;
	CurHealth = 100.0f;
	
	//MHUD = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	//MHUD->SetupAttachment(GetMesh());

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	 
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline")); 

	// 기본 설정
	bReplicates = true;

	// AI 자동 빙의 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	//Attack
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere")); 

	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
  
	OverlapSphere->SetSphereRadius(100.0f);
}

void ASMonster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASMonster, CurHealth);
} 
 
void ASMonster::BeginPlay()
{
	Super::BeginPlay();  

	AIControllerClass = AMonsterAIController::StaticClass();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackSphereOverlap);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCapsuleOverlap);
	}

	// AI 컨트롤러가 없다면 생성
	if (HasAuthority() && !GetController() && MAIControllerClass)
	{

		SpawnDefaultController();

		//UE_LOG(LogTemp, Warning, TEXT("Create Class"));
		//FActorSpawnParameters SpawnParams;
		//SpawnParams.Owner = this;
		//AAIController* AIController = GetWorld()->SpawnActor<AAIController>(MAIControllerClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	}

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceivedDamage);
		
	}
}

void ASMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	
	//FVector Forward = GetActorForwardVector();  
	// 
	//AddMovementInput(Forward);

}

void ASMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASMonster::OffMonster()
{
	//PrimaryActorTick.bCanEverTick = false;

	MaxHealth = 100.0f;
	CurHealth = 100.0f;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetActive(false);
	GetCharacterMovement()->SetActive(false);
}

void ASMonster::OnMonster(FVector location)
{ 

	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 100.0f;
	CurHealth = 100.0f;

 
	SetActorLocation(location);

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetCharacterMovement()->SetActive(true);
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->GravityScale = 5.0f;

	GetMesh()->SetActive(true);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	
	if(GetMesh()->GetAnimInstance() ==nullptr)
		UE_LOG(LogTemp, Warning, TEXT("Anum Error"));

}


/*
Damaged Received
*/
void ASMonster::ReceivedDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	ASCharacter* User = Cast<ASCharacter>(InstigatedBy->GetCharacter());
	if (User == nullptr)
	{
		CurHealth = FMath::Clamp(CurHealth - Damage, 0.0f, MaxHealth);	 
	}
	// User가 가한 데미지 면 넉백
	else
	{
		CurHealth = FMath::Clamp(CurHealth - Damage, 0.0f, MaxHealth);	 
		FVector dir = GetActorLocation() - DamageCauser->GetActorLocation();
		dir.Normalize();
		UE_LOG(LogTemp, Warning, TEXT("Impulse: %.2f * %.2f"), dir.Length(), Damage);

		 LaunchCharacter(dir * Damage * KnockBack, true, false);

		//GetCharacterMovement()->AddImpulse(dir * Damage * KnockBack,true);
	}

	//UpdateHUDHealth();
	//React Montage 

	//Elim
	if (CurHealth <= 0)
		Elim();
}

/*
Health
*/

void ASMonster::OnRep_CurHealth(float LastHealth)
{ 
	//UpdateHUDHealth();

	if (CurHealth < LastHealth)
	{
		//PlayHitReactMontage();
	}
}


/*
Udate HUD 
*/

void ASMonster::UpdateHUDHealth()
{
	// Get the actual user widget from HealthWidget and cast it to UMonsterHUD
	//UMonsterHUD* MHUD = Cast<UMonsterHUD>(HealthHUD); 

	//HealthHUD = Cast<UMonsterHUD>(MHUD->GetUserWidgetObject());
	//
	//if (HealthHUD)
	//{
	//	HealthHUD->UpdateHealth(MaxHealth, CurHealth);
	//}c
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("MHUD not found"));
	//}
}

void ASMonster::MulticastElim_Implementation()
{
	bElimmed = true;

	PlayElimMontage();

	//disable Character Monvement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	 
	 
	//start dissolve effect
	if (DissolveMaterialInstance)
	{ 
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);

		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), -0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.0f);
	}

	//dissolve
	StartDissolve();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//Gen Drop Item
	int32 randNum = FMath::RandRange(0, 100);

	int total = 0;
	for (auto num : Items)
	{
		total += num.Value;
		if (randNum < total)
		{
			FActorSpawnParameters params;
			FVector location = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 100);
			GetWorld()->SpawnActor<AActor>(num.Key, location, GetActorRotation(), params);
			break;
		}
	} 
}
void ASMonster::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissloveMaterial); 
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ASMonster::Elim()
{
	MulticastElim();
}

void ASMonster::PlayElimMontage()
{		
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	 
	if (AnimInstance && ElimReactMontage)
	{   
		AnimInstance->Montage_Play(ElimReactMontage);
	}
	
}

void ASMonster::UpdateDissloveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}
 
void  ASMonster::OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ASCharacter* Player = Cast<ASCharacter>(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Attack OVerlap"));
	
	if (Player)
	{
	UE_LOG(LogTemp, Warning, TEXT("Attack Can"));
		Attack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack Cancel"));
	}
}
void ASMonster::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ 
	ASCharacter* Player = Cast<ASCharacter>(OtherActor);
	AController* InstigaterController = GetController();
	

	if (Player)
	{ 
		UGameplayStatics::ApplyDamage(
			Player,
			AttackDamage,
			InstigaterController,
			this,
			UDamageType::StaticClass()
		); UE_LOG(LogTemp, Warning, TEXT("Goet Damage"));
	}
}
void ASMonster::Attack()
{ 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackAnim)
	{ 
		AnimInstance->Montage_Play(AttackAnim);
	}
}
