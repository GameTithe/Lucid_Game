// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"

AMonsterAIController::AMonsterAIController()
{
    bReplicates = true; 
    bAlwaysRelevant = true;
    bAttachToPawn = true;
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();
    PrimaryActorTick.TickInterval = 0.5f; 
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds); 

    //���� Ÿ���õ� �÷��̾ ���ų� ���� �ð����� Ÿ���� ����
    if (!CurrentTarget || ShouldUpdateTarget())
    {
        Patrol();

        if (ClosestPlayer)
        {
            MoveToTarget(ClosestPlayer);
        }
    }
}

void AMonsterAIController::Patrol()
{ 
    float ClosestDistance = MAX_FLT;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PlayerController = It->Get();
        if (PlayerController && PlayerController->GetPawn())
        {
            float Distance = FVector::Distance(GetPawn()->GetActorLocation(),
                PlayerController->GetPawn()->GetActorLocation());

            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestPlayer = PlayerController->GetPawn();
            }
        }
    }
}

void AMonsterAIController::MoveToTarget(AActor* Target)
{ 
    SetFocus(Target);
    MoveToActor(Target, 100.0f);
}

void AMonsterAIController::Attack()
{
}
