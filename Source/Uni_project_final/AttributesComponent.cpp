#include "AttributesComponent.h"
#include "Kismet/GameplayStatics.h"

UAttributesComponent::UAttributesComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    Health = MaxHealth;
    Stamina = MaxStamina;
}

void UAttributesComponent::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth;
    Stamina = MaxStamina;

    OnHealthChanged.Broadcast(Health, MaxHealth);
    OnStaminaChanged.Broadcast(Stamina, MaxStamina);
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    RegenerateStamina(DeltaTime);
}

void UAttributesComponent::SubtractHealth(float DamageAmount)
{
    if (DamageAmount <= 0.0f) return;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
            FString::Printf(TEXT("START HEALTH: %.1f, DAMAGE: %.1f"), Health, DamageAmount));
    }

    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

    OnHealthChanged.Broadcast(Health, MaxHealth);

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            HitSound,
            GetOwner()->GetActorLocation()
        );
    }

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
}

void UAttributesComponent::SetStamina(float NewStamina)
{
    Stamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
    OnStaminaChanged.Broadcast(Stamina, MaxStamina);
}

bool UAttributesComponent::CanPayStaminaCost(float Cost) const
{
    return Stamina >= Cost;
}

void UAttributesComponent::PayStamina(float Cost)
{
    if (Cost <= 0.0f) return;

    Stamina = FMath::Clamp(Stamina - Cost, 0.0f, MaxStamina);
    OnStaminaChanged.Broadcast(Stamina, MaxStamina);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            FString::Printf(TEXT("Stamina used: %.1f, Remaining: %.1f"), Cost, Stamina));
    }
}

void UAttributesComponent::RegenerateStamina(float DeltaTime)
{
    if (Stamina < MaxStamina)
    {
        float OldStamina = Stamina;
        Stamina = FMath::Clamp(Stamina + (StaminaCosts.StaminaRegenRate * DeltaTime), 0.0f, MaxStamina);

        if (OldStamina != Stamina)
        {
            OnStaminaChanged.Broadcast(Stamina, MaxStamina);
        }
    }
}