// Fill out your copyright notice in the Description page of Project Settings.


#include "SHUD.h"
#include "GameFramework/PlayerController.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "CharacterOverlay.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/HUD/Announcement.h"


void ASHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && AnnouncementClass && Announcement == nullptr)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void ASHUD::BeginPlay()
{
	Super::BeginPlay(); 
}

void ASHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ASHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		
		const FVector2D ViewportCenter(ViewportSize.X / 2.0f, ViewportSize.Y /2.0f);

		float SpreadScaled = CrosshairSpreadMax * CrosshairPackage.CrosshairSpread;

		if (CrosshairPackage.CrosshairCenter)
		{ 
			DrawCrosshair(CrosshairPackage.CrosshairCenter, ViewportCenter, FVector2D(0.0f, 0.0f), CrosshairPackage.LinearColor);
		}
		if (CrosshairPackage.CrosshairRight)
		{ 
			DrawCrosshair(CrosshairPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0.0f), CrosshairPackage.LinearColor);
		} 
		if (CrosshairPackage.CrosshairLeft)
		{ 
			DrawCrosshair(CrosshairPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.0f), CrosshairPackage.LinearColor);
		}
		if (CrosshairPackage.CrosshairTop)
		{  
			DrawCrosshair(CrosshairPackage.CrosshairTop, ViewportCenter, FVector2D(0.0f, -SpreadScaled), CrosshairPackage.LinearColor);
		}
		if (CrosshairPackage.CrosshairButtom)
		{ 
			DrawCrosshair(CrosshairPackage.CrosshairButtom, ViewportCenter, FVector2D(0.0f, SpreadScaled), CrosshairPackage.LinearColor);
		}
		 
	}
}

void ASHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color)
{
	float w = Texture->GetSizeX() ;
	float h = Texture->GetSizeY() ;
	FVector2D DrawPoint(ViewportCenter.X - w / 2.0f  + Spread.X, ViewportCenter.Y - h / 2.0f + Spread.Y);
	 
	DrawTexture(
		Texture,
		DrawPoint.X,
		DrawPoint.Y,
		w,
		h,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		Color
	);
}
