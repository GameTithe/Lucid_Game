// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	int32 NumOfPellets = 10;
};
