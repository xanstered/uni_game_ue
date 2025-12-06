#include "AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    Health = MaxHealth;
}

void UAttributesComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;
}

void UAttributesComponent::SubtractHealth(float DamageAmount)
{
    // Upewnienie siê, ¿e obra¿enia s¹ nieujemne
    if (DamageAmount <= 0.0f) return;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
            FString::Printf(TEXT("START HEALTH: %.1f, DAMAGE: %.1f"), Health, DamageAmount));
    }

    // Obliczanie nowego zdrowia i upewnienie siê, ¿e nie spadnie poni¿ej 0
    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
            FString::Printf(TEXT("!!! NEW HEALTH: %.1f / %.1f"), Health, MaxHealth));
    }

    if (Health <= 0.0f)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, TEXT("!!! ATTRIBUTES: HEALTH REACHED ZERO !!!"));
        }
        OnDeathDelegate.Broadcast();
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
                FString::Printf(TEXT("Health: %.1f / %.1f"), Health, MaxHealth));
        }
    }
}