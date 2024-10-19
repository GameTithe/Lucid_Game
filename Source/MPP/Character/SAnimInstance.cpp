// Fill out your copyright notice in the Description page of Project Settings.


#include "SAnimInstance.h"
#include "SCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MPP/Weapon/Weapon.h"
#include "MPP/SType/CombatState.h"


void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	SCharacter = Cast<ASCharacter>(TryGetPawnOwner());
}

void USAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SCharacter == nullptr)
	{
		SCharacter = Cast<ASCharacter>(TryGetPawnOwner());
	}
	if (SCharacter == nullptr) return;

	FVector Velocity = SCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	bIsInAir = SCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = SCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = SCharacter->IsWeaponEquipped();
	EquippedWeapon = SCharacter->GetEquippedWeapon();
	bIsCrouched = SCharacter->bIsCrouched;
	bIsAiming = SCharacter->IsAiming();
	TurningInPlace = SCharacter->GetTuringInPlace();
	bElimmed = SCharacter->IsElimmed();

	//OffSet Yaw for Standing
	FRotator AimRotator = SCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(SCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotator);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 5.0f);
	YawOffset = DeltaRotation.Yaw;

	//Lean
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = SCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);
	
	AO_Yaw = SCharacter->GetAO_Yaw();
	AO_Pitch = SCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && SCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		
		FVector OutPosition;
		FRotator OutRotation;

		SCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);

		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (SCharacter->IsLocallyControlled())
		{
			bIsLocally = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("RightHandSocket"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - SCharacter->GetHitTarget()));
		}

		//MuzzleTransform.GetRotation() 
	}

	//TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	bUseFABRIC = SCharacter->GetCombatState() != ECombatState::ECS_Reloading ? true : false;
	bUseAimOffset = SCharacter->GetCombatState() != ECombatState::ECS_Reloading  && !SCharacter->GetDisableGamePlay()  ? true : false;
	bRightHandTransform = SCharacter->GetCombatState() != ECombatState::ECS_Reloading && !SCharacter->GetDisableGamePlay() ? true : false;
}