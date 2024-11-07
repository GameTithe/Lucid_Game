// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsSGameMode.h"
#include "MPP/GameState/SGameState.h"
#include "MPP/PlayerState/SPlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATeamsSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	 
	ASGameState* SGameState = Cast<ASGameState>(UGameplayStatics::GetGameState(this)); 
	if (SGameState)
	{
		ASPlayerState* SPState = NewPlayer->GetPlayerState<ASPlayerState>();
		if (SPState && SPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (SGameState->BlueTeam.Num() <= SGameState->RedTeam.Num())
			{
				SGameState->BlueTeam.AddUnique(SPState);
				SPState->SetTeam(ETeam::ET_BlueTeam);
			}
			else
			{
				SGameState->RedTeam.AddUnique(SPState);
				SPState->SetTeam(ETeam::ET_RedTeam);
			}
		}
	}
}

void ATeamsSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ASGameState* SGameState = Cast<ASGameState>(UGameplayStatics::GetGameState(this));
	ASPlayerState* SPState = Exiting->GetPlayerState<ASPlayerState>();
	if (SGameState)
	{
		if (SGameState->RedTeam.Contains(SPState))
		{
			SGameState->RedTeam.Remove(SPState);
		}

		if (SGameState->BlueTeam.Contains(SPState))
		{
			SGameState->BlueTeam.Remove(SPState);
		}
	}
}

void ATeamsSGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	
	ASGameState* SGameState = Cast<ASGameState>( UGameplayStatics::GetGameState(this));
		
	if (SGameState)
	{
		for (auto player : SGameState->PlayerArray)
		{
			ASPlayerState* SPState = Cast<ASPlayerState>(player.Get());

			if (SPState && SPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (SGameState->BlueTeam.Num() <= SGameState->RedTeam.Num())
				{
					SGameState->BlueTeam.AddUnique(SPState);
					SPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					SGameState->RedTeam.AddUnique(SPState);
					SPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}

	}

}
