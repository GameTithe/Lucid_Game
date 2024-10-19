// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class MPP_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	 
	ACasing();

protected: 
	virtual void BeginPlay() override;
	virtual void Destroyed() override;


	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
	float EjectionImpulse;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComo, FVector NormalImpulse, const FHitResult& Hit);
private:

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;
	
};
