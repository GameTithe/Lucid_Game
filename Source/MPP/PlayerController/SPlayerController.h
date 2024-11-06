// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MPP_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CheckTimeSync(float DeltaTime);
	void PollInit();
	 
public: 
	UFUNCTION()
	void ActiveChatBox();

	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(const FString& msg);

	UFUNCTION(Client, Reliable)
	void ClientAddChatMessage(const FString& msg);


	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCount(float CountdownTime);
	void SetHUDAnnouncementCount(float CountdownTime); 
	void SetHUDTime();
	 
	void OnMatchStateSet(FName State);

	bool bInitHUD = false;

	void ShowReturnToMainMenu();
	 
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

protected:
	virtual void BeginPlay() override;
	virtual void ReceivedPlayer() override;
	virtual void SetupInputComponent() override;
	 
	/*
	Sync Time Between Server and Client 
	*/
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReprotServerTime(float TimeOfClientRequest, float TimeOfServerReport);

	float GetServerTime();
	float ServerClientDelta = 0.0f; 

	UPROPERTY(EditAnywhere)
	float TimeSyncFrequency = 5.0f;
	float TimeSyncRunningTime = 0.0f;

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Match, float Warmup,float Cooldown, float LevelStarting);

	UPROPERTY()
	class ASGameMode* SGameMode;

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

private:
	UPROPERTY()
	class ASHUD* SHUD; 

	/*
	Return to main menu
	*/
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

	float MatchTime = 0.0f;
	float WarmupTime = 0.0f;
	float CooldownTime = 0.0f;
	float LevelStartingTime;

	uint32 CountInt = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	void HandleMatchHasStarted();
	void HandleCooldown();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
;
	float HUDHealth;
	bool bInitHealth = false;
	 
	float HUDMaxHealth;
	float HUDScore;
	float HUDDefeats;

	float HUDCarriedAmmo;
	bool bInitCarriedAmmo = false;

	float HUDWeaponAmmo;
	bool bInitWeaponAmmo = false;

};
