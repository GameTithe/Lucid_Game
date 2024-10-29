// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPickup.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/ShooterComponent/BuffComponent.h" 

ASpeedPickup::ASpeedPickup()
{
}

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ASCharacter* SCharacter = Cast<ASCharacter>(OtherActor);
	if (SCharacter)
	{
		UBuffComponent* Buff = SCharacter->GetBuff();
		if (Buff)
		{ 
			Buff->BuffSpeeds(BaseSpeedBuff, CrouchSpeedBuff, AimSpeedBuff,SpeedBuffTime);
		}
	} 
	Destroy();
}