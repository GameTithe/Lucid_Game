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

    // Ÿ�� ������Ʈ�� �ʿ����� Ȯ���ϴ� �Լ�
    bool ShouldUpdateTarget()
    {
        // ���⿡ Ÿ�� ������Ʈ ������ �߰�
        // ��: ���� �ð�����, �Ÿ��� �ʹ� �־����� �� ��
        return !CurrentTarget || !IsValid(CurrentTarget);
    }
};
