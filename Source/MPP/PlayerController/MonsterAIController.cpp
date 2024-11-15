// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"


void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Player"));
		SetFocus(PlayerPawn);

		MoveToActor(PlayerPawn, 100);
	}
}