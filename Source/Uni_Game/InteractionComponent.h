#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNI_GAME_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInteractionComponent();

    // Zasiêg interakcji
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionDistance = 200.0f;

    // Funkcja sprawdzaj¹ca interakcje
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    AActor* CheckForInteractables();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    class ACharacter* OwnerCharacter;
};