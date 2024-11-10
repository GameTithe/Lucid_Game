// Fill out your copyright notice in the Description page of Project Settings.


#include "SMonster.h" 
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "MPP/HUD/MonsterHUD.h"
#include "MPP/MPP.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h" 
 
ASMonster::ASMonster()
{ 
	PrimaryActorTick.bCanEverTick = true; 

	MaxHealth = 100.0f;
	CurHealth = 100.0f;
	
	MHUD = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	MHUD->SetupAttachment(GetMesh());

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline")); 
}

void ASMonster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASMonster, CurHealth);
} 
 
void ASMonster::BeginPlay()
{
	Super::BeginPlay(); 

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceivedDamage);
	}
}
 

void ASMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/*
Damaged Received
*/
void ASMonster::ReceivedDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	UE_LOG(LogTemp, Warning, TEXT("ReceivedMonster Damage"));

	CurHealth = FMath::Clamp(CurHealth - Damage, 0.0f, MaxHealth);
	UpdateHUDHealth();

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
	UpdateHUDHealth();

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

	HealthHUD = Cast<UMonsterHUD>(MHUD->GetUserWidgetObject());

	if (HealthHUD)
	{
		HealthHUD->UpdateHealth(MaxHealth, CurHealth);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MHUD not found"));
	}
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
	
	 
}
void ASMonster::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissloveMaterial); 
	if (DissolveCurve && DissolveTimeline)
	{ ;
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
		UE_LOG(LogTemp, Warning, TEXT("ElimReact"));
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
 
