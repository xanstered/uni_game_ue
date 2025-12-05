#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"

// Delegat do powiadamiania o œmierci (lub utracie zdrowia)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNI_PROJECT_FINAL_API UAttributesComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttributesComponent();

private:
    UPROPERTY(EditAnywhere, Category = "Attributes")
    float MaxHealth = 75.0f;

    UPROPERTY(VisibleAnywhere, Category = "Attributes")
    float Health;

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SubtractHealth(float DamageAmount);

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnDeathSignature OnDeathDelegate;
};