// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h" 
#include "SMonster.generated.h"
UCLASS()
class MPP_API ASMonster : public ACharacter
{
	GENERATED_BODY()

public: 
	ASMonster();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected: 
	UPROPERTY(EditAnywhere)
	float KnockBack = 1;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMonsterHUD* HealthHUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* MHUD;



	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_CurHealth, VisibleAnywhere, Category = "Monster Stats")
	float CurHealth; 

	UFUNCTION()
	void OnRep_CurHealth(float LastHealth);

	UFUNCTION()
	void ReceivedDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void UpdateHUDHealth();

	/*
	Elim
	*/
	UFUNCTION()
	void UpdateDissloveMaterial(float DissolveValue);
	void StartDissolve();

	UPROPERTY(EditAnywhere)
	UAnimMontage* ElimReactMontage;
	
	void Elim();

	UFUNCTION(Category = "Elim")
	void PlayElimMontage();


/*
Dissolve
*/
	// Dynamic instance that  we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance  
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance;

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	bool bElimmed = false;


	/*
	DropItem
	*/ 
	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<AActor>, int32> Items;
	 
	 

public:	 
	virtual void Tick(float DeltaTime) override; 
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OffMonster();
	UFUNCTION()
	void OnMonster(FVector location);

	bool IsElimmed() { return bElimmed; }

	 
};
