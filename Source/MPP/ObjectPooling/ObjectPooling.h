// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPP/Monster/SMonster.h"
#include "Containers/Queue.h"

#include "ObjectPooling.generated.h"

UCLASS()
class MPP_API AObjectPooling : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectPooling();
	 
	UPROPERTY(EditAnywhere)
	int TestSpawnNum = 10;

	UPROPERTY(EditAnywhere)
	float bound = 500;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASMonster> Monster2;
	 
	// 0: min
	TQueue<ASMonster*> MonsterQueue;

	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<ASMonster>, int32> MonsterPooling;
	
	UPROPERTY(EditAnywhere)
	FVector location = FVector(0, 0, -500);

	UFUNCTION()
	void UseObject(int num);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override; 
};
