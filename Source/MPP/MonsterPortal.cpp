// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterPortal.h"
#include "ObjectPooling/ObjectPooling.h"

AMonsterPortal::AMonsterPortal()
{
 	PrimaryActorTick.bCanEverTick = true;

}

 void AMonsterPortal::BeginPlay()
{
	Super::BeginPlay();
	
}
  
 void AMonsterPortal::GenMonster()
 {
	 MonsterPooling->UseObject(10);
 }

 void AMonsterPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

