// Fill out your copyright notice in the Description page of Project Settings.


#include "SHUD.h"
#include "GameFramework/PlayerController.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "CharacterOverlay.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/HUD/Announcement.h"
#include "ElimAnnouncement.h"
#include "MPP/HUD/ChattingSystem.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MPP/HUD/Chatting.h"
#include "MPP/HUD/ChatMessage.h"
#include "Components/ScrollBox.h"

void ASHUD::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && ChattingClass)
	{ 
		Chatting = CreateWidget<UChatting>(PlayerController, ChattingClass);
		Chatting->AddToViewport();
	}  
}
void ASHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && AnnouncementClass && Announcement == nullptr)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}

}

void ASHUD::AddElimAnnouncement(FString Attacker, FString Victim)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
	if (OwningPlayer && ElimAnnouncementClass)
	{
		UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayer, ElimAnnouncementClass);
		if (ElimAnnouncementWidget)
		{
			ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();
			
			for (auto Msg : ElimMessages)
			{
				if (Msg && Msg->AnnouncementBox)
				{ 
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if (CanvasSlot)
					{
						FVector2D Position = CanvasSlot->GetPosition();

						FVector2D NewPosition(
							CanvasSlot->GetPosition().X,
							Position.Y - CanvasSlot->GetSize().Y
						);
						CanvasSlot->SetPosition(NewPosition);
					} }
			}
			ElimMessages.Add(ElimAnnouncementWidget);

			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgDelegate;
			
			ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(
				ElimMsgTimer,
				ElimMsgDelegate,
				ElimAnnouncementTime,
				false
			);

		}
	}
}

void ASHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)
{
	if (MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
	}
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

void ASHUD::AddChatting()
{
	if(Chatting)
	{
		Chatting->AddToViewport();
	}
}

void ASHUD::AddChatMessage(const FString& Message)
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
	Chatting = Chatting == nullptr ? CreateWidget<UChatting>(OwningPlayer, ChattingClass) : Chatting;

	UE_LOG(LogTemp, Warning, TEXT("Add CHat Msg"));
	if (OwningPlayer && ChattingClass && Chatting && ChatMessageClass)
	{
	UE_LOG(LogTemp, Warning, TEXT("Add CHat Msg2"));
		UChatMessage* ChatMessageWidget = CreateWidget<UChatMessage>(OwningPlayer, ChatMessageClass);

		if (ChatMessageWidget)
		{
	UE_LOG(LogTemp, Warning, TEXT("Add CHat Msg3"));
			ChatMessageWidget->SetChatMessage(Message);
			Chatting->ChatScrollBox->AddChild(ChatMessageWidget);
			Chatting->ChatScrollBox->ScrollToEnd();
			Chatting->ChatScrollBox->bAnimateWheelScrolling = true;
		}
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
