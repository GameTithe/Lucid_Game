// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes/TurningInPlace.h"
#include "MPP/Interface/CrosshairInterface.h"
#include "Components/TimelineComponent.h" 

#include "MPP/SType/CombatState.h"
#include "SCharacter.generated.h" 

UCLASS()
class MPP_API ASCharacter : public ACharacter, public ICrosshairInterface
{
	GENERATED_BODY()

public: 
	ASCharacter(); 
	virtual void Tick(float DeltaTime) override; 
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();
	void PlayElimMontage();

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	
	FVector GetHitTarget();

	void UpdateHUDHealth();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

protected: 
	virtual void BeginPlay() override;


	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Jump();
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();

	void PollInit();


	UFUNCTION()
	void ReceivedDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool bElimmed = false;
	

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon; 
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess ="true"))
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ElimReactMontage; 
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;


	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 100.0f;

	UPROPERTY()
	class ASPlayerState* SPlayerState;

	/*
	Health
	*/
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.0f; 
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.0f; 
	UFUNCTION()
	void OnRep_Health();
	 
	FTimerHandle ElimTimer;
	void ElimTimerFinished();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float ElimDelay = 3.0f;

	
	/*
	Dissolve Effect
	*/
	
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissloveMaterial(float DissolveValue);
	void StartDissolve();


	// Dynamic instance that  we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance  
	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* DissolveMaterialInstance;

	UPROPERTY()
	class ASPlayerController* SController;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);;
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTuringInPlace() const { return TurningInPlace;  }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE	bool IsElimmed() const { return bElimmed;  }
	FORCEINLINE UCombatComponent* GetCombat() { return Combat; }
	FORCEINLINE bool GetDisableGamePlay() { return bDisableGameplay; }
	
	ECombatState GetCombatState() const;
	

};
