// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBullet::AProjectileBullet()
{ 
	BulletMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BulletMovementComponent"));
	BulletMovementComponent->SetIsReplicated(true);
	BulletMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComo, FVector NormalImpulse, const FHitResult& Hit)
{ 
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	 
	if (OwnerCharacter)
	{ 
		AController* OwnerController = OwnerCharacter->GetController();
		if (OwnerController && HasAuthority())
		{ 
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}
	 

	Super::OnHit(HitComp, OtherActor, OtherComo, NormalImpulse,Hit);
}
