// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "Net/UnrealNetwork.h"

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASPlayerState, Defeats);

}

void ASPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	Character = Character == nullptr ? Cast<ASCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	} 
}

void ASPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;

	Character = Character == nullptr ? Cast<ASCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ASPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ASCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{ 
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ASPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ASCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ASPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
