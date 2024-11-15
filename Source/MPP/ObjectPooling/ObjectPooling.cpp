// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPooling.h" 
#include "MPP/Monster/SMonster.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"

AObjectPooling::AObjectPooling()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AObjectPooling::UseObject(int num)
{
	for (int i = 0; i < num; i++)
	{
		if (MonsterQueue.IsEmpty()) return;

		float offset = FMath::RandRange(-bound, bound);
		location = location + FVector(offset, 0, 0);

		ASMonster* Monster;
		if(MonsterQueue.Dequeue(Monster) && Monster!= nullptr)
			Monster->OnMonster(location);
	}
}

// Called when the game starts or when spawned
void AObjectPooling::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	for (auto m : MonsterPooling)
	{

		location = GetActorLocation();

		for (int i = 0; i < m.Value; i++)
		{
			FActorSpawnParameters params;
			ASMonster* Monster = GetWorld()->SpawnActor<ASMonster>(m.Key, location, GetActorRotation(), params);
			if (Monster != nullptr)
			{  
				//Monster->SpawnDefaultController();
				
				Monster->OffMonster();  
				MonsterQueue.Enqueue(Monster);
			}
		}
	}

	UseObject(TestSpawnNum);
}

void AObjectPooling::ManageMonster()
{
	if (!HasAuthority()) return;

	for (auto m : MonsterPooling)
	{

		location = GetActorLocation();

		for (int i = 0; i < m.Value; i++)
		{
			FActorSpawnParameters params; 

			ASMonster* Monster = GetWorld()->SpawnActor<ASMonster>(m.Key, location, GetActorRotation(), params);
			Monster->AIControllerClass = MAIControllerClass;
			Monster->SpawnDefaultController();  
			//Monster->OffMonster();
			MonsterQueue.Enqueue(Monster);
		}
	}

	//UseObject(TestSpawnNum);
}

// Called every frame
void AObjectPooling::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

