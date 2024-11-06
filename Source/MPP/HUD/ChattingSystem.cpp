// Fill out your copyright notice in the Description page of Project Settings.


#include "ChattingSystem.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "Components/Button.h"

//SendButton->OnClicked.AddDynamic(this, &) 

void UChattingSystem::SendChattingLog(FString Chatting)
{ 
    OwningController = OwningController == nullptr ? Cast<ASPlayerController>(GetOwningPlayer()) : OwningController;
   
   if(OwningController)
   {
       
   }
}
