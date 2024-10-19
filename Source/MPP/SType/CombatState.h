#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied	UMETA(Displayname = "Unoccupied"),
	ECS_Reloading	UMETA(Displayname = "Reloading"),

	ECS_DefaultMAX	UMETA(Displayname = "DefaultMAX")
};