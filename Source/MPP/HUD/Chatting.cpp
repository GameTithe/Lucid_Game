// Fill out your copyright notice in the Description page of Project Settings.


#include "Chatting.h"
#include "MPP/PlayerState/SPlayerState.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "Components/EditableText.h"

void UChatting::NativeConstruct()
{	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		ChatText->OnTextCommitted.AddDynamic(this, &UChatting::OnTextCommitted);
	}
	ChatText->SetIsEnabled(false);
}

void UChatting::ActivateChatText()
{
	if (ChatText)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chatting.ccp Activechattext"));
		ChatText->SetIsEnabled(true);
		ChatText->SetFocus();
	}
}

void UChatting::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{ 
	if (CommitMethod == ETextCommit::OnEnter)
	{ 
		if (ChatText)
		{
			// 좌우 공백 제거
			FText InputText = ChatText->GetText();
			FString TrimmedText = InputText.ToString().TrimStartAndEnd();

			if (!TrimmedText.IsEmpty())
			{
				ASPlayerController* PlayerController = Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController());
				if (PlayerController)
				{ 
					APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
					FString Message = FString::Printf(TEXT("%s : %s"), *PlayerState->GetPlayerName(), *TrimmedText);
					// 채팅 메시지를 보내기 위한 Server RPC 호출
					PlayerController->ServerSendChatMessage(Message);
					 
					FInputModeGameOnly InputMode;
					PlayerController->SetInputMode(InputMode);

					// 채팅창 비우고 비활성화
					ChatText->SetText(FText::GetEmpty());
					ChatText->SetIsEnabled(false);
				}
			}
		}
	}
}

