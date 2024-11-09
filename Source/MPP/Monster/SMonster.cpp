// Fill out your copyright notice in the Description page of Project Settings.


#include "SMonster.h" 
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "MPP/HUD/MonsterHUD.h"
#include "MPP/MPP.h"
 
ASMonster::ASMonster()
{ 
	PrimaryActorTick.bCanEverTick = true; 

	MaxHealth = 100.0f;
	CurHealth = 100.0f;
	
	MHUD = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	MHUD->SetupAttachment(GetMesh());

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
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
 
void ASMonster::OnRep_CurHealth(float LastHealth)
{
	//UpdateHealthHUID

	UE_LOG(LogTemp, Warning, TEXT("ONREP UpdateHUD"));
	UpdateHUDHealth();
	if (CurHealth < LastHealth)
	{
		//PlayHitReactMontage();
	}
}

//Server
void ASMonster::ReceivedDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{ 

	UE_LOG(LogTemp, Warning, TEXT("ReceivedMonster Damage"));

	CurHealth = FMath::Clamp(CurHealth - Damage, 0.0f, MaxHealth);
	UpdateHUDHealth();

	//React Montage 
	
	//Elim
}
void ASMonster::UpdateHUDHealth()
{
	// Get the actual user widget from HealthWidget and cast it to UMonsterHUD
	//UMonsterHUD* MHUD = Cast<UMonsterHUD>(HealthHUD);
	UE_LOG(LogTemp, Warning, TEXT("UpdateHUD1"));

	HealthHUD = Cast<UMonsterHUD>(MHUD->GetUserWidgetObject());

	if (HealthHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateHUD2"));
		HealthHUD->UpdateHealth(MaxHealth, CurHealth);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MHUD not found"));
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

