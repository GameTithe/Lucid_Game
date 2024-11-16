// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    AMonsterAIController();
protected:

    virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
private:

    UPROPERTY()
    APawn* CurrentTarget;
    
    UFUNCTION()
    void Patrol();

    UFUNCTION()
    void MoveToTarget(AActor* Target);

    UPROPERTY()
    APawn* ClosestPlayer = nullptr;
    
    UFUNCTION()
    void Attack();

    // 타겟 업데이트가 필요한지 확인하는 함수
    bool ShouldUpdateTarget()
    {
        // 여기에 타겟 업데이트 조건을 추가
        // 예: 일정 시간마다, 거리가 너무 멀어졌을 때 등
        return !CurrentTarget || !IsValid(CurrentTarget);
    }
};
