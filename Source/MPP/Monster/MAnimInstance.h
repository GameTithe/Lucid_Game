// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SMonster.h"
#include "MAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API UMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(BlueprintReadOnly, Category = "Monster", meta = (AllowPrivateAccess  = "true" ))
	ASMonster* SMonster;

	UPROPERTY(BlueprintReadOnly, Category ="Monster", meta = (AllowPrivateAccess = "true"))
	bool bElimmed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling = false;

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	void StopMomnet();

};
