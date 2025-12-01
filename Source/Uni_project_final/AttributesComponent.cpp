#include "AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    Health = MaxHealth;
}

void UAttributesComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAttributesComponent::SubtractHealth(float DamageAmount)
{
    // Upewnienie siê, ¿e obra¿enia s¹ nieujemne
    if (DamageAmount <= 0.0f) return;

    // Obliczanie nowego zdrowia i upewnienie siê, ¿e nie spadnie poni¿ej 0
    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

    if (Health <= 0.0f)
    {
        // Broadcastowanie delegata œmierci
        OnDeathDelegate.Broadcast();

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
                FString::Printf(TEXT("Character Died!")));
        }
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