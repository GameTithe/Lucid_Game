// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SGameMode.generated.h"

/**
 * 
 */

namespace MatchState
{
	extern MPP_API const FName Cooldown;
}
UCLASS()
class MPP_API ASGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ASGameMode();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnMatchStateSet() override;

	virtual void PlayerEliminated(class ASCharacter* ElimmedCharacter, class APlayerController* VictimController, APlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter,AController* ElimmedController);

	void SendChatMessage(const FString& Message);

	void PlayerLeftGame(class ASPlayerState* PlayerLeaving);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.0f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float LevelStartingTime = 0.0f;


protected:
	float CountDownTime = 0.0f;
	
public:
	FORCEINLINE float GetCountDownTime() const { return CountDownTime;}
};
 