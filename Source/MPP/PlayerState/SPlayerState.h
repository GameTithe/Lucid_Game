// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public :
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

	
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

private:
	UPROPERTY()
	class ASCharacter* Character;
	UPROPERTY()
	class ASPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};