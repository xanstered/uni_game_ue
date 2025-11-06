#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface2.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionInterface2 : public UInterface
{
    GENERATED_BODY()
};

class UNI_GAME_API IInteractionInterface2
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void PickUp(AActor* PlayerCharacter);

    virtual void PickUp_Implementation(AActor* PlayerCharacter);
};