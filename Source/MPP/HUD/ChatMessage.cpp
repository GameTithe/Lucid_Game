// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatMessage.h"
#include "Components/TextBlock.h"

void UChatMessage::SetChatMessage(const FString& Messgae)
{
	if (ChatMessageTextBlock)
	{
		ChatMessageTextBlock->SetText(FText::FromString(Messgae));
		ChatMessageTextBlock->Font.Size = 16;
	}
}
