#pragma once

#include "CoreMinimal.h"
#include "PawnStateEnum.generated.h"

UENUM(BlueprintType)
enum class EPawnState : uint8
{
    E_Idle          UMETA(DisplayName = "Idle"),
    E_Combat        UMETA(DisplayName = "In Combat"),
    E_Hit           UMETA(DisplayName = "Hit"),
    E_Occupied      UMETA(DisplayName = "Occupied"),
    E_Dead          UMETA(DisplayName = "Dead"),
    E_Exhausted     UMETA(DisplayName = "Exhausted")
};