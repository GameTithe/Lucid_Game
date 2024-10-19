// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);
  
	UPROPERTY(EditAnywhere)
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParicle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticle;
public:
	virtual void Fire(const FVector& HitTarget) override;


	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	class USoundCue* FireSound;
	
	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

};
