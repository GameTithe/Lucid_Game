#pragma once

#define TRACE_LENGTH 8000.0f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssultRifle UMETA(DisplayName = "Assult Rifle"),
	EWT_RocketLancher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_ShotGun UMETA(DisplayName = "ShotGun"),
	EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),
	
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};


