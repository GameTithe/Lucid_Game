// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterPortal.generated.h"

UCLASS()
class MPP_API AMonsterPortal : public AActor
{
	GENERATED_BODY()
	
public:	 
	AMonsterPortal();

protected: 
	virtual void BeginPlay() override; 
		
	UPROPERTY(EditAnywhere)
	TMap<int32, int32> WavesSetting;

	UPROPERTY(EditAnywhere)
	class AObjectPooling* MonsterPooling;
		
	void GenMonster();

public:
	virtual void Tick(float DeltaTime) override;

};
