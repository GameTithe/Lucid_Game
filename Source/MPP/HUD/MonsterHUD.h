// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHUD.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API UMonsterHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

public:
	UFUNCTION()
	void UpdateHealth(float MaxHealth, float CurHealth);
};
