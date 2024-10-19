// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h" 
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
}
 
void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * EjectionImpulse);
	
	SetLifeSpan(1.0f);
}
void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComo, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	
	CasingMesh->SetNotifyRigidBodyCollision(false);
}


void ACasing::Destroyed()
{
	Super::Destroyed();

}

