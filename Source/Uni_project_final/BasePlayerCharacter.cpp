#include "BasePlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InteractionComponent.h"
#include "InteractionInterface.h"
#include "Weapon.h"
#include "AttributesComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"

ABasePlayerCharacter::ABasePlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(TEXT("AttributesComponent"));

    CurrentWeapon = nullptr;
    WeaponSocketName = FName("WeaponSocket");

    CombatState = EPlayerState::E_Idle; 
}

void ABasePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (AttributesComponent)
    {
        AttributesComponent->OnDeathDelegate.AddDynamic(this, &ABasePlayerCharacter::HandleDeath);
    }
}

void ABasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Move);
        }

        if (AttackAction)
        {
            EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABasePlayerCharacter::Attack);
        }

        if (InteractAction)
        {
            EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ABasePlayerCharacter::Interact);
        }
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Look);
        }
    }
}

void ABasePlayerCharacter::SetPlayerState(EPlayerState NewState)  // ZMIENIONE
{
    CombatState = NewState;  // ZMIENIONE

    if (GEngine)
    {
        FString StateString;
        switch (NewState)
        {
        case EPlayerState::E_Idle: StateString = "Idle"; break;  // ZMIENIONE
        case EPlayerState::E_Combat: StateString = "Combat"; break;  // ZMIENIONE
        case EPlayerState::E_Hit: StateString = "Hit"; break;  // ZMIENIONE
        case EPlayerState::E_Occupied: StateString = "Occupied"; break;  // ZMIENIONE
        case EPlayerState::E_Dead: StateString = "Dead"; break;  // ZMIENIONE
        }

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
            FString::Printf(TEXT("Player Combat State: %s"), *StateString));
    }
}

bool ABasePlayerCharacter::CanPerformAttack() const
{
    return (CombatState == EPlayerState::E_Idle || CombatState == EPlayerState::E_Combat)  // ZMIENIONE
        && CombatState != EPlayerState::E_Hit  // ZMIENIONE
        && CombatState != EPlayerState::E_Occupied  // ZMIENIONE
        && CombatState != EPlayerState::E_Dead;  // ZMIENIONE
}

void ABasePlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ABasePlayerCharacter::Attack(const FInputActionValue& Value)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Attack pressed"));
    }

    if (CurrentWeapon && CanPerformAttack())
    {
        PlayAttackMontage();
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
                TEXT("Cannot attack - no weapon or wrong state"));
        }
    }
}

void ABasePlayerCharacter::PlayAttackMontage()
{
    if (AttackMontage && CanPerformAttack())
    {
        SetPlayerState(EPlayerState::E_Occupied); 

        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(AttackMontage);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Playing attack animation"));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No AttackMontage or can't attack"));
        }
    }
}

void ABasePlayerCharacter::Interact(const FInputActionValue& Value)
{
    if (InteractionComponent)
    {
        AActor* InteractableActor = InteractionComponent->GetInteractableActor();

        if (InteractableActor)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                    FString::Printf(TEXT("Found: %s"), *InteractableActor->GetName()));
            }

            AWeapon* Weapon = Cast<AWeapon>(InteractableActor);
            if (Weapon)
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("it's a weapon"));
                }

                Weapon->PickUp_Implementation(this);
                EquipWeapon(Weapon);
            }
            else
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("cast failed"));
                }
            }
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("nothing to interact with"));
            }
        }
    }
}

void ABasePlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ABasePlayerCharacter::DoLook(float Yaw, float Pitch)
{
    if (GetController() != nullptr)
    {
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

void ABasePlayerCharacter::EquipWeapon(AWeapon* NewWeapon)
{
    if (!NewWeapon)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("NewWeapon is NULL"));
        return;
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->Destroy();
    }

    CurrentWeapon = NewWeapon;

    USkeletalMeshComponent* CharacterMesh = GetMesh();
    if (!CharacterMesh)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No CharacterMesh"));
        return;
    }

    if (CharacterMesh->DoesSocketExist(WeaponSocketName))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                FString::Printf(TEXT("Socket EXISTS: %s"), *WeaponSocketName.ToString()));
        }

        CurrentWeapon->EquipWeapon(this, CharacterMesh);
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange,
                FString::Printf(TEXT("Socket NOT FOUND: %s"), *WeaponSocketName.ToString()));
        }
    }
}

void ABasePlayerCharacter::PerformWeaponAttackTrace()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Trace initiated by Character"));
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->PerformWeaponTrace();
    }
}

void ABasePlayerCharacter::GetHit_Implementation(float DamageAmount)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
            FString::Printf(TEXT("=== PLAYER HIT! Damage: %.2f ==="), DamageAmount));
    }

    if (CombatState == EPlayerState::E_Dead) 
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Player already dead"));
        }
        return;
    }

    if (AttributesComponent)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
                FString::Printf(TEXT("Health before: %.1f"), AttributesComponent->GetHealth()));
        }

        AttributesComponent->SubtractHealth(DamageAmount);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
                FString::Printf(TEXT("Health after: %.1f"), AttributesComponent->GetHealth()));
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("ERROR: No AttributesComponent!"));
        }
    }

    if (CombatState != EPlayerState::E_Dead) 
    {
        SetPlayerState(EPlayerState::E_Hit);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Player state set to HIT"));
        }
    }
}

void ABasePlayerCharacter::HandleDeath()
{
    SetPlayerState(EPlayerState::E_Dead);  

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (GetMovementComponent())
    {
        GetMovementComponent()->StopMovementImmediately();
    }

    DisableInput(Cast<APlayerController>(GetController()));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("=== PLAYER DEAD ==="));
    }
}