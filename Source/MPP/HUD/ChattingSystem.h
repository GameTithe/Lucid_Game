// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChattingSystem.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API UChattingSystem : public UUserWidget
{
public: 
	GENERATED_BODY()
	
	UFUNCTION()
	void SendChattingLog(FString Chatting); 
	 
	UPROPERTY(meta = (BindWidget))
	class UEditableText* ChatText;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* ChatBox; 

	UPROPERTY(meta = (BindWidget))
	class UButton* SendButton;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ChatList;

private:
	UPROPERTY()
	class ASPlayerController* OwningController;

	UPROPERTY()
	class ASPlayerState* PlayerState;
};
