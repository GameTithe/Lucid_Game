// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include  "Components/WidgetComponent.h"
#include "MPP/Character/SCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "MPP/PlayerController/SPlayerController.h"

AWeapon::AWeapon()
{
 	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);	 
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);	
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{ 
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
}
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASCharacter* SCharacter = Cast<ASCharacter>(OtherActor);
	if (SCharacter)
	{ 
		SCharacter->SetOverlappingWeapon(this);
	}

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASCharacter* SCharacter = Cast<ASCharacter>(OtherActor);
	if (SCharacter)
	{
		SCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);		
		if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		break;
	case EWeaponState::EWS_Dropped:  
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		break;
	}

}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
}
 
void AWeapon::OnRep_Ammon()
{ 
	SetHUDAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		SOwnerCharacter = nullptr;
		SOwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

void AWeapon::SetHUDAmmo()
{ 
	SOwnerCharacter = SOwnerCharacter == nullptr ? Cast<ASCharacter>(GetOwner()) : SOwnerCharacter;
	if (SOwnerCharacter)
	{ 
		SOwnerController = SOwnerController == nullptr ? Cast<ASPlayerController>(SOwnerCharacter->Controller) : SOwnerController;
		if (SOwnerController)
		{ 
			SOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}


void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch(WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SubmachineGun )
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		break;

	case EWeaponState::EWS_Dropped: 
		if (HasAuthority())
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		break;
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{ 
	if (PickupWidget)
	{ 
		PickupWidget->SetVisibility(bShowWidget);
	}
}

//Server
void AWeapon::Fire(const FVector& HitTarget)
{
	if(FireAnimation)
		WeaponMesh->PlayAnimation(FireAnimation, false);
	
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			FRotator RandRotator = UKismetMathLibrary::RandomRotator();
			FRotator LerpRot = UKismetMathLibrary::RLerp(SocketTransform.Rotator(), RandRotator, 0.3f, true);
			UWorld* World = GetWorld(); 
			
			if (World)
			{ 
				World->SpawnActor<ACasing>(
					BulletCasing,
					SocketTransform.GetLocation(),
					LerpRot);
			}
		 
	if (BulletCasing)
	{
		}
	}

	SpendRound();
}
 

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped); 
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	SOwnerCharacter = nullptr;
	SOwnerController = nullptr;

}

void AWeapon::AmmoToAdd(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}
