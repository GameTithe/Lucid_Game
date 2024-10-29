// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPP/Character/SCharacter.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MPP_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class ASCharacter; 
	void Heal(float HealAmount, float HealingTime);
	void BuffSpeeds(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffAimSpeed, float BuffTime);
	void SetinitSpeed(float BaseSpeed, float CrouchSpeed, float );

protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);

private:
	UPROPERTY()
	ASCharacter* Character;

	/*
		Heal Buff
	*/
	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	/*
		Speed Buff
	*/
	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitBaseSpeed;
	float InitCrouchSpeed;
	float InitAimSpeed;
	bool bSpeedBuff = false;

	float CurBaseSpeed;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed, float AimSpeed);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//FORCEINLINE float GetBaseSpeed() {return }
	FORCEINLINE	bool GetIsSpeedBuff() { return bSpeedBuff; }
};
