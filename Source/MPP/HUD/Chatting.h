// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chatting.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API UChatting : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* ChatText;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ChatScrollBox;

	UFUNCTION()
	void ActivateChatText();

	UFUNCTION() 
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
};
