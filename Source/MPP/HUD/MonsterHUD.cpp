// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterHUD.h"
#include "Components/ProgressBar.h"
void UMonsterHUD::UpdateHealth(float MaxHealth, float CurHealth)
{ 
	if (HealthBar)
	{
		if (MaxHealth == 0) HealthBar->Percent = 0.0f;
		else
		{
			HealthBar->SetPercent(CurHealth / MaxHealth);
			UE_LOG(LogTemp, Warning, TEXT("%.2f"), (CurHealth / MaxHealth));
		}
	}
}
