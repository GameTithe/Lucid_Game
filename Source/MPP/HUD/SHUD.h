// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairButtom;

	float CrosshairSpread;
	FLinearColor LinearColor;
};

/**
 * 
 */
UCLASS()
class MPP_API ASHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;
		
	void AddAnnouncement();

protected:
	virtual void BeginPlay() override; 

private:
	void DrawCrosshair(class UTexture2D* texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color);
	FHUDPackage CrosshairPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.0f;
public:
	FORCEINLINE void SetCrosshair(const FHUDPackage& Package) { CrosshairPackage = Package; }
};
