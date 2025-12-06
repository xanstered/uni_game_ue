#include "BaseEnemyCharacter.h"
#include "AttributesComponent.h"
#include "Components/CapsuleComponent.h" 
#include "GameFramework/PawnMovementComponent.h" 
#include "Components/SkeletalMeshComponent.h" 
#include "Animation/AnimMontage.h"
#include "CombatInterface.h" 

ABaseEnemyCharacter::ABaseEnemyCharacter()
{
    AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(TEXT("AttributesComponent"));
    AttributesComponent->PrimaryComponentTick.bCanEverTick = false;
    PawnState = EPawnState::E_Idle;
}

void ABaseEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (AttributesComponent)
    {
        AttributesComponent->OnDeathDelegate.AddDynamic(this, &ABaseEnemyCharacter::HandleDeath);
    }
}

void ABaseEnemyCharacter::SetPawnState(EPawnState NewState)
{
    PawnState = NewState;
}

void ABaseEnemyCharacter::HandleDeath()
{
    SetPawnState(EPawnState::E_Dead);

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (GetMovementComponent())
    {
        GetMovementComponent()->StopMovementImmediately();
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, TEXT("ENEMY: DED"));
    }
}

void ABaseEnemyCharacter::GetHit_Implementation(float DamageAmount)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta,
            FString::Printf(TEXT("=== GetHit CALLED! Damage: %.2f ==="), DamageAmount));
    }

    if (PawnState == EPawnState::E_Dead)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Already dead, ignoring"));
        }
        return;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
            FString::Printf(TEXT("AttributesComponent pointer: %p"), AttributesComponent));
    }

    if (AttributesComponent)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
                FString::Printf(TEXT("BEFORE SubtractHealth - Current Health: %.1f"),
                    AttributesComponent->GetHealth()));
        }

        AttributesComponent->SubtractHealth(DamageAmount);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green,
                FString::Printf(TEXT("AFTER SubtractHealth - New Health: %.1f"),
                    AttributesComponent->GetHealth()));
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
                TEXT("ERROR: AttributesComponent is NULL!!!"));
        }
    }

    if (PawnState != EPawnState::E_Dead)
    {
        SetPawnState(EPawnState::E_Hit);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
                FString::Printf(TEXT("ENEMY: HIT! State: %s"), *UEnum::GetValueAsString(PawnState)));
        }
    }
}

bool ABaseEnemyCharacter::CanPerformAttack() const
{
    return (PawnState == EPawnState::E_Combat || PawnState == EPawnState::E_Idle) && PawnState != EPawnState::E_Hit && PawnState != EPawnState::E_Occupied && PawnState != EPawnState::E_Dead;
}

void ABaseEnemyCharacter::StartAttack()
{
    if (CanPerformAttack())
    {
        SetPawnState(EPawnState::E_Occupied);

        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && AttackMontage)
        {
            float Duration = AnimInstance->Montage_Play(AttackMontage);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Magenta, TEXT("ENEMY: Attacking!"));
            }
        }
        else
        {
            SetPawnState(EPawnState::E_Combat);
        }
    }
}

void ABaseEnemyCharacter::StopCurrentAttack()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
    {
        AnimInstance->Montage_Stop(0.1f);
    }

    if (PawnState == EPawnState::E_Occupied)
    {
        SetPawnState(EPawnState::E_Combat);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ENEMY ATTACK INTERRUPTED!"));
        }
    }
}