#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

USTRUCT(BlueprintType)
struct FStaminaCost
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float StaminaRegenRate = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float StaminaCost_Attack = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float StaminaCost_Jump = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float StaminaCost_Sprint = 5.0f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNI_PROJECT_FINAL_API UAttributesComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttributesComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* HitSound;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnDeathSignature OnDeathDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Attributes")
    FOnStaminaChanged OnStaminaChanged;

private:
    UPROPERTY(EditAnywhere, Category = "Attributes")
    float MaxHealth = 75.0f;

    UPROPERTY(VisibleAnywhere, Category = "Attributes")
    float Health = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Attributes")
    float MaxStamina = 100.0f;

    UPROPERTY(VisibleAnywhere, Category = "Attributes")
    float Stamina = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    FStaminaCost StaminaCosts;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SubtractHealth(float DamageAmount);

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetStamina() const { return Stamina; }

    UFUNCTION(BlueprintPure, Category = "Attributes")
    float GetMaxStamina() const { return MaxStamina; }

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void SetStamina(float NewStamina);

    UFUNCTION(BlueprintPure, Category = "Attributes")
    bool CanPayStaminaCost(float Cost) const;

    UFUNCTION(BlueprintCallable, Category = "Attributes")
    void PayStamina(float Cost);

    UFUNCTION(BlueprintPure, Category = "Attributes")
    FStaminaCost GetStaminaCosts() const { return StaminaCosts; }

private:
    void RegenerateStamina(float DeltaTime);
};