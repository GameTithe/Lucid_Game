// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPP/HUD/SHUD.h"
#include "MPP/Weapon/WeaponTypes.h"   
#include "MPP/SType/CombatState.h"

#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MPP_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	 
	UCombatComponent();
	friend class ASCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	Weapon
	*/
	void EquipWeapon(class AWeapon* WeaponToEquip);

	UPROPERTY(EditAnywhere)
	bool bCanFire = true;

	void FireButtonPressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	void PickupAmmo(EWeaponType type, int32 AmmoAmount);

	/*
	Speed
	*/
	void SetAimSpeed(float SpeedOfAim) { AimWalkSpeed = SpeedOfAim; } 
	FORCEINLINE void SetBaseWalkSpeed(float speed) { BaseWalkSpeed = speed; }
	
protected: 
	virtual void BeginPlay() override;
	void SetAiming(bool bisAiming);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();


	void Fire();
	
	void Reload();
	int32 AmountToReload();

	UFUNCTION(BlueprintCallable)
	void FinishedReload();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	//Server&Client
	void HandleReload();

	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantize TraceHitTarget);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire (FVector_NetQuantize TraceHitTarget);

	void TraceUnderCrossHair(FHitResult& TraceHitResult);

	void SetHUDCrosshair(float DeltaTime);


	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float FOV;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float SpeedFOV;


private:
	UPROPERTY()
	class ASCharacter* Character;
	UPROPERTY()
	class ASPlayerController* Controller;
	class ASHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming; 

	UPROPERTY(EditAnyWhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnyWhere)
	float AimWalkSpeed;

	bool bFireButtonpressed; 

	/*
	HUD
	*/
	FHUDPackage HUDPackage;
	float CrossHairSpreadFactor;
	float CrossHairJumpFactor; 
	float CroosHairAimFactor;
	float CroosHairShootFactor;
	FVector HitTarget;

	/*
	Zoom FOV
	*/ 
	float DefaultFOV;
	float CurFOV;

	void InterpFOV(float DeltaTime);
	UPROPERTY(EditAnyWhere)
	float ZoomedFOV = 30.0f;
	UPROPERTY(EditAnyWhere)
	float ZoomInterpFOV = 20.0f;

	/*
	Automatic Fire
	*/ 
	FTimerHandle FireTimer; 

	void StartFireTimer();
	void FinishFireTimer();

	void EmptyGunSound();

	bool CanFire();
	
	// Carried Ammo for the currently-equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	//Ammo container
	
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 4;
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 15;
	UPROPERTY(EditAnywhere)
	int32 StartingSubmachineGun = 20;
	UPROPERTY(EditAnywhere)
	int32 StartingShotGun = 5;
	UPROPERTY(EditAnywhere)
	int32 StartingSniperfile = 5;

	void InitializedCarriedAmmo();
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState;// = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();
	
	void UpdateCarriedAmmo();
	void UpdateShotgunAmmoValue();

};

