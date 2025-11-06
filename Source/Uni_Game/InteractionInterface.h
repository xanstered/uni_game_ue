#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * W³aœciwy interfejs — funkcje do zaimplementowania w klasach
 */
class UNI_GAME_API IInteractionInterface
{
    GENERATED_BODY()

public:
    // Funkcja wywo³ywana przy interakcji ( naciœniêcie "E")
    virtual void Interact(AActor* Interactor) = 0;
};