 
#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	 
	if (!HasAuthority()) return;

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget =  HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		if (ProjectileClass)
		{ 
			UWorld* World = GetWorld();
			if (World)
			{ 
				FActorSpawnParameters SpawnParam;
				SpawnParam.Owner = GetOwner();
				SpawnParam.Instigator = InstigatorPawn;

				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParam
				);
			}
		}
	}  
}
