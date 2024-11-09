// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameMode.h"
#include "TeamsSGameMode.generated.h"

/**
 * 
 */
class AController;
class APlayerController;
UCLASS()
class MPP_API ATeamsSGameMode : public ASGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
	virtual void HandleMatchHasStarted() override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;

};
