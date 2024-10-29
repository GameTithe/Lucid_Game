// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/ShooterComponent/BuffComponent.h" 

AHealthPickup::AHealthPickup()
{
	bReplicates = true;
}


void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ASCharacter* SCharacter = Cast<ASCharacter>(OtherActor);
	if (SCharacter)
	{
		UBuffComponent* Buff =  SCharacter->GetBuff();
		if (Buff)
		{
			Buff->Heal(HealAmount, HealingTime);
		}
	}

	Destroy();
}
 