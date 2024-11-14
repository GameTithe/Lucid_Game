// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" 
#include "WeaponTypes.h"

#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial Name"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "EWS_EquippedSecondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "Default MAX")
};

UCLASS()
class MPP_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	 
	AWeapon(); 
	virtual void Tick(float DeltaTime) override; 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	virtual void OnEquippedSecondary();

	void SetHUDAmmo();

	void ShowPickupWidget(bool bShowWidget);
	
	virtual void Fire(const FVector& HitTarget);

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	UTexture2D* CrosshairsBottom;

	bool bDestroyWeapon = false;
	/*
	custom depth
	*/
	void EnableCustomDepth(bool bEnable);
protected: 
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();

	virtual void OnEquipped();
	virtual void OnDropped();


	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap( 
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);  
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;
	
	UPROPERTY(Replicatedusing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget; 

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;
	 
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> BulletCasing;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammon)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammon();
	
	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;


	UPROPERTY()
	class ASCharacter* SOwnerCharacter;
	
	UPROPERTY()
	class ASPlayerController* SOwnerController;

public:	 

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.0f;

	UPROPERTY(EditAnywhere)
	float ZoomedInterpSpeed = 20.0f;

	void SetWeaponState(EWeaponState State);
	FORCEINLINE	USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE	USkeletalMeshComponent* GetWeaponMesh() { return WeaponMesh; }
	FORCEINLINE	EWeaponType GetWeaponType() { return WeaponType; }
	FORCEINLINE	int32 GetAmmo() { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() { return MagCapacity; }

	bool IsEmpty();
	bool IsFull();

	void Dropped();
	void AmmoToAdd(int32 AmmoToAdd);

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;
	UPROPERTY(EditAnywhere)
	class USoundWave* EmptyGunSound;

	/*/
	Automatic Fire
	*/
	 
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;
	 
};
