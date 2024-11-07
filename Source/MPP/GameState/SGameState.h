// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SGameState.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API ASGameState : public AGameState
{
	GENERATED_BODY()

public:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class ASPlayerState* ScoringPlayer);
	
	UPROPERTY(Replicated)
	TArray<ASPlayerState*> TopScoringPlayers;

	/*
	Teams
	*/
	TArray<ASPlayerState*> RedTeam;
	TArray<ASPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
	float RedTeamScore = 0.f;
	UFUNCTION()
	void OnRep_RedTeamScore();

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;
	UFUNCTION()
	void OnRep_BlueTeamScore();

	
private:
	float TopScore = 0.0f;

};
