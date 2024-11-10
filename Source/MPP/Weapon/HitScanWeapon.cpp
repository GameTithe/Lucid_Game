// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "MPP/Character/SCharacter.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigaterController = OwnerPawn->GetController();
	 
		// InstigaterController is not going to be valid on a simulated proxy
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start,HitTarget, FireHit);  

		//ASCharacter* SCharacter = Cast<ASCharacter>(FireHit.GetActor());
		ACharacter* DamagedCharacter = Cast<ACharacter>(FireHit.GetActor());
		UWorld* World = GetWorld(); 
		if (DamagedCharacter && HasAuthority() && InstigaterController)
		{ 
			UGameplayStatics::ApplyDamage(
				DamagedCharacter,
				Damage,
				InstigaterController,
				this,
				UDamageType::StaticClass()
			);
		}

		if (World && HitSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				World,
				HitSound,
				FireHit.ImpactPoint
			);
		}

		if (ImpactParicle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParicle,
				FireHit.ImpactPoint,
				FireHit.ImpactPoint.Rotation()
			);
		}

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{

			UGameplayStatics::SpawnSoundAtLocation(
				GetWorld(),
					FireSound,
					SocketTransform.GetLocation()
				);
		}
	}

} 
void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld(); 
	if (World)
	{
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
			BeamEnd = OutHit.ImpactPoint;
		if (BeamParticle)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticle,
				TraceStart
			);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
		 
	}
}
 
FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector SphereCenter = TraceStart + DistanceToSphere * (HitTarget - TraceStart).GetSafeNormal();
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.0f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVec;
	FVector ToEndLoc = EndLoc - TraceStart;

	/*
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.0f, 12, FColor::Orange, true);
	
	DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()), FColor::Cyan, true);
	*/
	return  FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
