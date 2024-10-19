// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "GameFramework/GameState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumOfPlayer = GameState.Get()->PlayerArray.Num();

	if (NumOfPlayer == 2 )
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			//게임 모드에는 servermode 밖에 없고 servermode를 이동하면 client도 같이 이동한다. 
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
}
