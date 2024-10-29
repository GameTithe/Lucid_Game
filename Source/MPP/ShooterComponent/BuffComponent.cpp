// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h" 
#include "MPP/ShooterComponent/CombatComponent.h"
#include "MPP/Character/SCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{ 
	PrimaryComponentTick.bCanEverTick = true;
	 
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
} 
  
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
	 
	
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (bHealing == false || Character == nullptr || Character->IsElimmed() ) return;

	const float HealthThisFrame = HealingRate * DeltaTime;
	AmountToHeal -= HealthThisFrame;
	
	Character->SetHealth(Character->GetHealth() + HealthThisFrame);
	Character->UpdateHUDHealth();

	if (AmountToHeal <= 0.0f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.0f;
	}
}


void UBuffComponent::BuffSpeeds(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffAimSpeed, float BuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeeds,
		BuffTime
		);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
		Character->GetCombat()->SetAimSpeed(BuffAimSpeed);
		Character->GetCombat()->SetBaseWalkSpeed(BuffBaseSpeed);
		bSpeedBuff = true;
	}

	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed, BuffAimSpeed);
}
 
void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || !Character->GetCharacterMovement()) return;
	
	Character->GetCharacterMovement()->MaxWalkSpeed = InitBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitCrouchSpeed;
	Character->GetCombat()->SetAimSpeed(InitAimSpeed);
	Character->GetCombat()->SetBaseWalkSpeed(InitBaseSpeed);
	bSpeedBuff = false;

	MulticastSpeedBuff(InitBaseSpeed, InitCrouchSpeed, InitAimSpeed);
}

void UBuffComponent::SetinitSpeed(float BaseSpeed, float CrouchSpeed, float AimSpeed) {
	InitBaseSpeed = BaseSpeed;
	InitCrouchSpeed = CrouchSpeed;
	InitAimSpeed = AimSpeed; 
	bSpeedBuff = false;
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed, float AimSpeed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	Character->GetCombat()->SetAimSpeed(AimSpeed);
	Character->GetCombat()->SetBaseWalkSpeed(BaseSpeed);
	bSpeedBuff = true;
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRampUp(DeltaTime);
}

