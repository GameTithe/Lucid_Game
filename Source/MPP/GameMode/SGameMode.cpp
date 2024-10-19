// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "MPP/PlayerState/SPlayerState.h"
#include "MPP/GameState/SGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}
ASGameMode::ASGameMode()
{
	bDelayedStart = true;
}

void ASGameMode::BeginPlay()
{
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ASGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		
		if(CountDownTime <= 0.0f)
		{
			StartMatch();
		}
	}

	else if (MatchState == MatchState::InProgress)
	{
		CountDownTime = MatchTime + WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.0f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}

	else if (MatchState == MatchState::Cooldown)
	{
		CountDownTime = CooldownTime + MatchTime + WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.0f)
		{
			RestartGame();
		}
	}	
}

void ASGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	 
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ASPlayerController* SPlayerController = Cast<ASPlayerController>(*It);
		if (SPlayerController)
		{
			SPlayerController->OnMatchStateSet(MatchState);
		}
	} 
}

//For Server
void ASGameMode::PlayerEliminated(ASCharacter* ElimmedCharacter, APlayerController* VictimController, APlayerController* AttackerController)
{
	ASPlayerState* AttackerPlayetState = AttackerController ? Cast<ASPlayerState>(AttackerController->PlayerState) : nullptr;
	ASPlayerState* VictimPlayerState = VictimController ? Cast<ASPlayerState>(VictimController->PlayerState) : nullptr;

	ASGameState* SGameState = GetGameState<ASGameState>();

	if (AttackerPlayetState && (AttackerPlayetState != VictimPlayerState) && SGameState)
	{
		AttackerPlayetState->AddToScore(1.0f);
		SGameState->UpdateTopScore(AttackerPlayetState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{ 
		ElimmedCharacter->Elim();
	}
}

void ASGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> StartPoints; 
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), StartPoints);

		int32 Selection  = FMath::RandRange(0, StartPoints.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, StartPoints[Selection]);
	}
}