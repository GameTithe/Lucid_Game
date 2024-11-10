// Fill out your copyright notice in the Description page of Project Settings.


#include "MAnimInstance.h"

void UMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SMonster = Cast<ASMonster>(TryGetPawnOwner());
}

void UMAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (SMonster == nullptr)
	{
		SMonster = Cast<ASMonster>(TryGetPawnOwner());
	}

	if (SMonster == nullptr) return;

	bElimmed = SMonster->IsElimmed();
}
