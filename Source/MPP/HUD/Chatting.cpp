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
		ChatText->SetIsEnabled(false);
	}
}

void UChatting::ActivateChatText()
{
	if (ChatText)
	{
		ChatText->SetIsEnabled(true);
		ChatText->SetFocus();
	}
}

void UChatting::DeactiveChatText(ASPlayerController* Controller)
{
	if (ChatText && Controller)
	{
		ChatText->SetText(FText::GetEmpty());
		ChatText->SetIsEnabled(false);

		FInputModeGameOnly InputMode;
		Controller->SetInputMode(InputMode);
	}
}

void UChatting::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && ChatText)
	{ 
		FText InputText = ChatText->GetText();

		if (!InputText.IsEmpty())
		{
			ASPlayerController* PlayerController = Cast<ASPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PlayerController)
			{
				APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
				FString Message = FString::Printf(TEXT("%s: %s"), *PlayerState->GetPlayerName(), *InputText.ToString());

				// ä�� �޽����� ������ ���� Server RPC ȣ��
				PlayerController->ServerSendChatMessage(Message);
				DeactiveChatText(PlayerController);
			}
		}
	}

}

