// StalkerTypes.h
#pragma once

#include "CoreMinimal.h"
#include "StalkerTypes.generated.h"

UENUM(BlueprintType)
enum class EStalkerState : uint8
{
	Init      UMETA(DisplayName="Init"),  
	Idle        UMETA(DisplayName="Idle"),
	Patrol      UMETA(DisplayName="Patrol"),
	Chase       UMETA(DisplayName="Chase"),
	Attack      UMETA(DisplayName="Attack"),
	Cooldown    UMETA(DisplayName="Cooldown")
};