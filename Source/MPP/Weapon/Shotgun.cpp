// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "MPP/Character/SCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget); 
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigaterController = OwnerPawn->GetController();

	// NO Controller IN Client When Server shoot gun
	// InstigaterController is not going to be valid on a simulated proxy
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		uint32 Hits = 0;
		TMap<ASCharacter*, uint32> HitMap;
		for (int i = 0; i < NumOfPellets; i++)
		{
			//FVector End = TraceEndWithScatter(Start, HitTarget);

			FHitResult HitResult;
			WeaponTraceHit(Start, HitTarget, HitResult);


			ASCharacter* Character = Cast<ASCharacter>(HitResult.GetActor());
			if (Character && HasAuthority() && InstigaterController)
			{
				if (HitMap.Contains(Character))
				{
					HitMap[Character]++;
				}
				else
				{
					HitMap.Emplace(Character, 1);

				}
			}

			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					GetWorld(),
					HitSound,
					HitResult.ImpactPoint,
					0.5f,
					FMath::RandRange(-0.5f, 0.5f)
				);
			}

			if (ImpactParicle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParicle,
					HitResult.ImpactPoint,
					HitResult.ImpactPoint.Rotation()
				);
			}
		}

		for (auto& hitBill : HitMap)
		{
			if (HasAuthority() && hitBill.Key && InstigaterController)
			{
				UGameplayStatics::ApplyDamage(
					hitBill.Key,
					Damage * hitBill.Value,
					InstigaterController,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
}
