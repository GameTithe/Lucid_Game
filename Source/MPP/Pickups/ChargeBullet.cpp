// Fill out your copyright notice in the Description page of Project Settings.
#include "ChargeBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MPP/Character/SCharacter.h" 
#include "Components/SphereComponent.h" 
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "MPP/Weapon/Weapon.h"
#include "MPP/ShooterComponent/CombatComponent.h"

AChargeBullet::AChargeBullet()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
    OverlapBox->SetupAttachment(RootComponent);
    OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OverlapBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    OverlapBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    ChargeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChargetMesh"));
    ChargeMesh->SetupAttachment(RootComponent);
    ChargeMesh->SetRenderCustomDepth(true);
    ChargeMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);

    bRender = true;
}

void AChargeBullet::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxOverlap);

        MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_AssultRifle, MaxAR);
        MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLancher, MaxRocket);
        MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, MaxPistol);
        MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, MaxSubMGun);
        MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_ShotGun, MaxShotGun);
        MaxCarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, MaxSniper);
    }

    UpdateRenderState();
}

void AChargeBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AChargeBullet, bRender);
}

void AChargeBullet::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;

    ASCharacter* SCharacter = Cast<ASCharacter>(OtherActor);
    bool check = SCharacter &&
        SCharacter->GetCombat() &&
        SCharacter->IsWeaponEquipped();

    if (check && bRender)
    {
        EWeaponType type = SCharacter->GetEquippedWeapon()->GetWeaponType();
        if (MaxCarriedAmmoMap.Contains(type))
        {
            int32 amount = MaxCarriedAmmoMap[type];
            SCharacter->GetCombat()->SetCarriedAmmo(amount);

            bRender = false;
            UpdateRenderState();

            // Start the recharge timer on server
            GetWorldTimerManager().SetTimer(
                RenderTimer,
                this,
                &ThisClass::RenderTimerFinished,
                ChargeDelay
            );
             
        }
    }
}

void AChargeBullet::OnRep_Render(bool LastRenderCheck)
{
    UpdateRenderState();
 }

void AChargeBullet::UpdateRenderState()
{
    ChargeMesh->SetRenderCustomDepth(bRender);
}

void AChargeBullet::RenderTimerFinished()
{
    if (!HasAuthority()) return;

    bRender = true;
    UpdateRenderState();
     
}

void AChargeBullet::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
