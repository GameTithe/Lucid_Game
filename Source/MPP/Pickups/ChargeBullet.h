// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPP/Weapon/WeaponTypes.h"

#include "ChargeBullet.generated.h"

UCLASS()
class MPP_API AChargeBullet : public AActor
{
	GENERATED_BODY()

public:	 
	AChargeBullet();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(ReplicatedUsing = OnRep_Render)
	bool bRender = true;

	UFUNCTION()
	void OnRep_Render(bool LastRenderCheck);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ChargeMesh;


	UPROPERTY(EditAnywhere)
	int32 MaxPistol = 50;
	UPROPERTY(EditAnywhere)
	int32 MaxSubMGun = 50;
	UPROPERTY(EditAnywhere)
	int32 MaxAR = 30;
	UPROPERTY(EditAnywhere)
	int32 MaxRocket = 5;
	UPROPERTY(EditAnywhere)
	int32 MaxShotGun = 5;
	UPROPERTY(EditAnywhere)
	int32 MaxSniper = 5;

	float cooldown = 10;
	
	UFUNCTION()
	void UpdateRenderState();

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* OverlapBox;

	TMap<EWeaponType, int32 > MaxCarriedAmmoMap;

	FTimerHandle RenderTimer;
	void RenderTimerFinished();

	UPROPERTY(EditAnywhere)
	float ChargeDelay = 3.0f;




	 
public:	 
	virtual void Tick(float DeltaTime) override;

};
