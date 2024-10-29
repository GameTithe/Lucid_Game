// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "MPP/HUD/SHUD.h"
#include "MPP/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MPP/PlayerController/SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "MPP/GameMode/SGameMode.h"  
#include "MPP/HUD/CharacterOverlay.h"
#include "MPP/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "MPP/GameMode/SGameMode.h"
#include "MPP/Character/SCharacter.h"
#include "MPP/ShooterComponent/CombatComponent.h" 
#include "MPP/GameState/SGameState.h"
#include "MPP/PlayerState/SPlayerState.h"


void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SHUD = Cast<ASHUD>(GetHUD()); 

	ServerCheckMatchState();
}
	
void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerController, MatchState);
	 
}
void ASPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}

}

void ASPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{ 
	float CurServerTime = GetWorld()->GetTimeSeconds();

	ClientReprotServerTime(TimeOfClientRequest, CurServerTime);
}

void ASPlayerController::ClientReprotServerTime_Implementation(float TimeOfClientRequest, float TimeOfServerReport)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeOfServerReport + (0.5 * RoundTripTime);

	ServerClientDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ASPlayerController::GetServerTime()
{ 
	if(HasAuthority()) return GetWorld()->GetTimeSeconds();

	return GetWorld()->GetTimeSeconds() + ServerClientDelta;
}

void ASPlayerController::ServerCheckMatchState_Implementation()
{ 
	ASGameMode* GameMode = Cast<ASGameMode>(UGameplayStatics::GetGameMode(this));

	if (GameMode)
	{
		MatchState = GameMode->GetMatchState();
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		WarmupTime = GameMode->WarmupTime;
		LevelStartingTime = GameMode->LevelStartingTime;

		ClientJoinMidgame(MatchState, MatchTime, WarmupTime, CooldownTime, LevelStartingTime);
	
		if (SHUD && MatchState == MatchState::WaitingToStart)
		{
			SHUD->AddAnnouncement();
		}
	}
}

void ASPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Match, float Warmup, float Cooldown, float LevelStarting)
{
	MatchState = StateOfMatch;
	MatchTime = Match;
	WarmupTime = Warmup;
	CooldownTime = Cooldown;
	LevelStartingTime = LevelStarting; 
	OnMatchStateSet(MatchState); 
	
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Alsp [layer"));
	}
	else
	{
		if(IsLocalController())
			UE_LOG(LogTemp, Warning, TEXT("client Alsp [layer"));
	}

	if (SHUD && MatchState == MatchState::WaitingToStart)
	{
		SHUD->AddAnnouncement();
	}
}



void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);

	PollInit();
}

void ASPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{  
		 
		if (SHUD && SHUD->CharacterOverlay)
		{  
			CharacterOverlay = SHUD->CharacterOverlay; 
			if(CharacterOverlay)
			{  
				if(bInitHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				//SetHUDDefeats(HUDDefeats);
				//SetHUDScore(HUDScore);
				if (bInitCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
			}
 
		}
	}
}

void ASPlayerController::CheckTimeSync(float DeltaTime)
{

	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncFrequency < TimeSyncRunningTime)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

void ASPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	SHUD = SHUD == nullptr? Cast<ASHUD>(GetHUD()) : SHUD;

	bool isValid =
		SHUD &&
		SHUD->CharacterOverlay &&
		SHUD->CharacterOverlay->HealthBar &&
		SHUD->CharacterOverlay->HealthText;
	 
	if (isValid)
	{
		const float HealthPercent = Health / MaxHealth;
		 SHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent); 
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		SHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
		 
	}
	else
	{  
		bInitHUD = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ASPlayerController::SetHUDScore(float Score)
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;

	bool isValid =
		SHUD &&
		SHUD->CharacterOverlay&&
		SHUD->CharacterOverlay->ScoreAmount;

	if (isValid)
	{

		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		SHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	} 
	else
	{
		bInitHUD = true;
		HUDScore = Score;
	}
}

void ASPlayerController::SetHUDDefeats(int32 Defeats)
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;

	bool bHUDValid =
		SHUD &&
		SHUD->CharacterOverlay &&
		SHUD->CharacterOverlay->DefeatsAmount;
	
	if (bHUDValid)
	{ 
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		SHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitHUD = true;
		HUDDefeats = Defeats;
	}

}

void ASPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;

	bool isValid =
		SHUD &&
		SHUD->CharacterOverlay &&
		SHUD->CharacterOverlay->WeaponAmmoAmount;

	if (isValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		SHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ASPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;

	bool isValid =
		SHUD &&
		SHUD->CharacterOverlay&&
		SHUD->CharacterOverlay->CarriedAmmoAmount;

	if (isValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		SHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	} 
	else
	{
		bInitCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void ASPlayerController::SetHUDMatchCount(float CountdownTime)
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;
	
	bool isValid =
		SHUD &&
		SHUD->CharacterOverlay &&
		SHUD->CharacterOverlay->MatchCountText;

	if (isValid)
	{
		if(CountdownTime < 0.0f)
		{
			FString CountText = FString::Printf(TEXT(""));
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60.0f;

		FString CountText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		SHUD->CharacterOverlay->MatchCountText->SetText(FText::FromString(CountText));
	} 
}

void ASPlayerController::SetHUDAnnouncementCount(float CountdownTime)
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;

	bool isValid =
		SHUD &&
		SHUD->Announcement &&
		SHUD->Announcement->WarmupTime;

	if (isValid)
	{
		if (CountdownTime < 0.0f)
		{
			SHUD->Announcement->WarmupTime->SetText(FText::FromString(""));
			return; 
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60.0f;

		FString CountText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		SHUD->Announcement->WarmupTime->SetText(FText::FromString(CountText));
	}
}

void ASPlayerController::SetHUDTime()
{
	if (HasAuthority())
	{
		ASGameMode* GameMode = Cast<ASGameMode>(UGameplayStatics::GetGameMode(this));
		if (GameMode)
		{
			LevelStartingTime = GameMode->LevelStartingTime;
		}
	}

	float LeftTime = 0.0f; 
	if (MatchState == MatchState::WaitingToStart) LeftTime = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) LeftTime = MatchTime + WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) LeftTime = CooldownTime + MatchTime + WarmupTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(LeftTime);

	// 서버가 로비에 오래있으면 시간이 뒤틀릴 수 있다 (Controller 재생성하는 방법도 있다)
	if (HasAuthority())
	{	
		SGameMode = SGameMode == nullptr ? Cast<ASGameMode>(UGameplayStatics::GetGameMode(this)) : SGameMode;
		if (SGameMode)
		{
			LeftTime = SGameMode->GetCountDownTime();
			SecondsLeft = FMath::CeilToInt32(LeftTime);
			//SecondsLeft = FMath::CeilToInt(SGameMode->GetCountDownTime() + LevelStartingTime);
		} 
	}	

	if (CountInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCount(LeftTime);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCount(LeftTime);
		} 
	}
	CountInt = SecondsLeft;
}
void ASPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State; 
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
} 

void ASPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ASPlayerController::HandleMatchHasStarted()
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;
	if (SHUD)
	{
		if(SHUD->CharacterOverlay== nullptr) 
			SHUD->AddCharacterOverlay();
		if (SHUD->Announcement)
		{
			SHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		} 
	}
}

void ASPlayerController::HandleCooldown()
{
	SHUD = SHUD == nullptr ? Cast<ASHUD>(GetHUD()) : SHUD;

	if (SHUD)
	{
		SHUD->CharacterOverlay->RemoveFromParent();
	
		bool IsValid = SHUD &&
			SHUD->Announcement &&
			SHUD->Announcement->AnnouncementText &&
			SHUD->Announcement->InfoText; 
		if (IsValid)
		{
			SHUD->Announcement->SetVisibility(ESlateVisibility::Visible);

			FString AnnouncementText("New Match Start in");
			SHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ASGameState* SGameState = Cast<ASGameState>(UGameplayStatics::GetGameState(this));
			ASPlayerState* SPlayerState = GetPlayerState<ASPlayerState>();

			if (SGameState)
			{
				TArray<ASPlayerState*> TopPlayers = SGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is No Winner");
				}
				else if (TopPlayers.Num() == 1  && TopPlayers[0] == SPlayerState)
				{
					InfoTextString = FString("You are Top Ranker");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Top Ranker is \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win: \n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}

				SHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	} 
	ASCharacter* DisableCharacter = Cast<ASCharacter>(GetPawn());
	if (DisableCharacter)
	{
		DisableCharacter->bDisableGameplay = true;
		DisableCharacter->GetCombat()->FireButtonPressed(false);
	}
}