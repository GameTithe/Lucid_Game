// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoPickup.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/ShooterComponent/CombatComponent.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor , OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	ASCharacter* SCharacter = Cast<ASCharacter>(OtherActor);
	if (SCharacter)
	{
		UCombatComponent* Combat = SCharacter->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}

	Destroy();
}
