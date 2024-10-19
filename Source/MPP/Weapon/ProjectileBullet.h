// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
	AProjectileBullet();

protected:

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* BulletMovementComponent;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComo, FVector NormalImpulse, const FHitResult& Hit) override;

};
