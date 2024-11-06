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



	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ChattingClass; 
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ChatMessageClass; 
	UPROPERTY(EditAnywhere, Category = "Annoucements")
	class UChatting* Chatting;

	void AddChatting();
	void AddChatMessage(const FString& Message);


	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;
		
	void AddAnnouncement();
	void AddElimAnnouncement(FString Attacker, FString Victim);

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> ChattingSystemClass;

	UPROPERTY()
	class UChattingSystem* ChattingSystem;


protected:
	virtual void BeginPlay() override; 

private:
	UPROPERTY()
	class APlayerController* OwningPlayer;
	
	void DrawCrosshair(class UTexture2D* texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color);
	FHUDPackage CrosshairPackage;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.0f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass; 

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 3.0f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;

public:
	FORCEINLINE void SetCrosshair(const FHUDPackage& Package) { CrosshairPackage = Package; }
};
