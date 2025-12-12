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
#include "MainHUD.h"
#include "BasePlayerController.h"

ABasePlayerCharacter::ABasePlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(TEXT("AttributesComponent"));

    CurrentWeapon = nullptr;
    WeaponSocketName = FName("WeaponSocket");
    CombatState = EPawnState::E_Idle;
    PlayerHUD = nullptr;
    bIsSprinting = false;
}

void ABasePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (AttributesComponent)
    {
        AttributesComponent->OnDeathDelegate.AddDynamic(this, &ABasePlayerCharacter::HandleDeath);
    }

    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->MaxWalkSpeed = WalkSpeed;
    }

    FTimerHandle HUDSetupTimer;
    GetWorld()->GetTimerManager().SetTimer(
        HUDSetupTimer,
        this,
        &ABasePlayerCharacter::SetupHUD,
        0.1f,
        false
    );
}

void ABasePlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateHUDState();

    if (bIsSprinting && AttributesComponent)
    {
        float SprintCost = AttributesComponent->GetStaminaCosts().StaminaCost_Sprint;

        if (AttributesComponent->CanPayStaminaCost(SprintCost * DeltaTime))
        {
            AttributesComponent->PayStamina(SprintCost * DeltaTime);
        }
        else
        {
            bIsSprinting = false;

            if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
            {
                MovementComp->MaxWalkSpeed = WalkSpeed;
            }

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
                    TEXT("OUT OF STAMINA - Sprint stopped!"));
            }

            if (PlayerHUD)
            {
                PlayerHUD->ShowLowStaminaWarning();
            }
        }
    }
}

void ABasePlayerCharacter::SetupHUD()
{
    ABasePlayerController* PC = Cast<ABasePlayerController>(GetController());
    if (PC && PC->CurrentHUD)
    {
        PlayerHUD = PC->CurrentHUD;

        if (AttributesComponent)
        {
            PlayerHUD->BindToAttributesComponent(AttributesComponent);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                    TEXT("Player: HUD connected to AttributesComponent"));
            }
        }
    }
    else
    {
        FTimerHandle RetryTimer;
        GetWorld()->GetTimerManager().SetTimer(RetryTimer, this, &ABasePlayerCharacter::SetupHUD, 0.2f, false);
    }
}

void ABasePlayerCharacter::UpdateHUDState()
{
    if (PlayerHUD)
    {
        FString StateString;
        switch (CombatState)
        {
        case EPawnState::E_Idle: StateString = "Idle"; break;
        case EPawnState::E_Combat: StateString = "Combat"; break;
        case EPawnState::E_Hit: StateString = "Hit"; break;
        case EPawnState::E_Occupied: StateString = "Occupied"; break;
        case EPawnState::E_Dead: StateString = "Dead"; break;
        case EPawnState::E_Exhausted: StateString = "Exhausted"; break;
        }

        if (bIsSprinting)
        {
            StateString += " (Sprinting)";
        }

        PlayerHUD->UpdatePawnState(StateString);
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

        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABasePlayerCharacter::Jump);
        }
        if (SprintAction)
        {
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABasePlayerCharacter::StartSprint);
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABasePlayerCharacter::StopSprint);
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ABasePlayerCharacter::StopSprint);
        }
    }
}

void ABasePlayerCharacter::SetPlayerState(EPawnState NewState)
{
    CombatState = NewState;

    if (GEngine)
    {
        FString StateString;
        switch (NewState)
        {
        case EPawnState::E_Idle: StateString = "Idle"; break;
        case EPawnState::E_Combat: StateString = "Combat"; break;
        case EPawnState::E_Hit: StateString = "Hit"; break;
        case EPawnState::E_Occupied: StateString = "Occupied"; break;
        case EPawnState::E_Dead: StateString = "Dead"; break;
        case EPawnState::E_Exhausted: StateString = "Exhausted"; break;
        }

        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
            FString::Printf(TEXT("Player Combat State: %s"), *StateString));
    }
}

bool ABasePlayerCharacter::CanPerformAttack() const
{
    return (CombatState == EPawnState::E_Idle || CombatState == EPawnState::E_Combat)
        && CombatState != EPawnState::E_Hit
        && CombatState != EPawnState::E_Occupied
        && CombatState != EPawnState::E_Dead
        && CombatState != EPawnState::E_Exhausted;
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

    if (!CurrentWeapon)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("No weapon equipped"));
        }
        return;
    }

    if (!CanPerformAttack())
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Cannot attack - wrong state"));
        }
        return;
    }

    if (AttributesComponent)
    {
        float AttackCost = AttributesComponent->GetStaminaCosts().StaminaCost_Attack;

        if (!AttributesComponent->CanPayStaminaCost(AttackCost))
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
                    TEXT("NOT ENOUGH STAMINA FOR ATTACK!"));
            }

            if (PlayerHUD)
            {
                PlayerHUD->ShowLowStaminaWarning();
            }

            SetPlayerState(EPawnState::E_Exhausted);
            FTimerHandle ExhaustedTimer;
            GetWorld()->GetTimerManager().SetTimer(
                ExhaustedTimer,
                [this]()
                {
                    if (CombatState == EPawnState::E_Exhausted)
                    {
                        SetPlayerState(EPawnState::E_Idle);
                    }
                },
                0.5f,
                false
            );

            return;
        }

        AttributesComponent->PayStamina(AttackCost);
    }

    PlayAttackMontage();
}

void ABasePlayerCharacter::Jump(const FInputActionValue& Value)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Jump pressed"));
    }

    if (CombatState == EPawnState::E_Dead || CombatState == EPawnState::E_Hit)
    {
        return;
    }

    if (!GetCharacterMovement()->IsFalling())
    {
        if (AttributesComponent)
        {
            float JumpCost = AttributesComponent->GetStaminaCosts().StaminaCost_Jump;

            if (!AttributesComponent->CanPayStaminaCost(JumpCost))
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
                        TEXT("NOT ENOUGH STAMINA TO JUMP!"));
                }

                if (PlayerHUD)
                {
                    PlayerHUD->ShowLowStaminaWarning();
                }

                return;
            }

            AttributesComponent->PayStamina(JumpCost);
        }

        ACharacter::Jump();

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Jumped!"));
        }
    }
}

void ABasePlayerCharacter::StartSprint(const FInputActionValue& Value)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            FString::Printf(TEXT("Sprint START - setting speed to %.1f"), SprintSpeed));
    }

    if (CombatState == EPawnState::E_Dead || CombatState == EPawnState::E_Hit)
    {
        return;
    }

    if (AttributesComponent)
    {
        if (!AttributesComponent->CanPayStaminaCost(1.0f)) 
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
                    TEXT("NOT ENOUGH STAMINA TO SPRINT!"));
            }

            if (PlayerHUD)
            {
                PlayerHUD->ShowLowStaminaWarning();
            }

            return;
        }
    }

    bIsSprinting = true;

    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->MaxWalkSpeed = SprintSpeed;

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                FString::Printf(TEXT("Sprint ACTIVE - MaxWalkSpeed: %.1f"), MovementComp->MaxWalkSpeed));
        }
    }
}

void ABasePlayerCharacter::StopSprint(const FInputActionValue& Value)
{
    if (!bIsSprinting)
    {
        return; 
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            FString::Printf(TEXT("Sprint stopped - setting speed to %.1f"), WalkSpeed));
    }

    bIsSprinting = false;

    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->MaxWalkSpeed = WalkSpeed;

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
                FString::Printf(TEXT("Current MaxWalkSpeed: %.1f"), MovementComp->MaxWalkSpeed));
        }
    }
}

void ABasePlayerCharacter::PlayAttackMontage()
{
    if (AttackMontage && CanPerformAttack())
    {
        SetPlayerState(EPawnState::E_Occupied);

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
        CurrentWeapon->EquipWeapon(this, CharacterMesh);
    }
}

void ABasePlayerCharacter::PerformWeaponAttackTrace()
{
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

    if (CombatState == EPawnState::E_Dead)
    {
        return;
    }

    if (bIsSprinting)
    {
        StopSprint(FInputActionValue());
    }

    if (AttributesComponent)
    {
        AttributesComponent->SubtractHealth(DamageAmount);
    }

    if (CombatState != EPawnState::E_Dead)
    {
        SetPlayerState(EPawnState::E_Hit);
        PlayHitMontage();
    }
}

void ABasePlayerCharacter::PlayHitMontage()
{
    if (HitMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(HitMontage);
        }
    }
}

void ABasePlayerCharacter::HandleDeath()
{
    SetPlayerState(EPawnState::E_Dead);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, TEXT("PLAYER: DEAD"));
    }

    if (bIsSprinting)
    {
        StopSprint(FInputActionValue());
    }

    PlayDeathMontage();

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (GetMovementComponent())
    {
        GetMovementComponent()->StopMovementImmediately();
    }

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        DisableInput(PC);
    }

    FTimerHandle HideTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        HideTimerHandle,
        [this]() { SetActorHiddenInGame(true); },
        2.5f,
        false
    );
}

void ABasePlayerCharacter::PlayDeathMontage()
{
    if (DeathMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DeathMontage);
        }
    }
}

void ABasePlayerCharacter::AnimNotify_AttackTrace()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("AnimNotify: AttackTrace"));
    }
    PerformWeaponAttackTrace();
}

void ABasePlayerCharacter::AnimNotify_AttackEnd()
{
    SetPlayerState(EPawnState::E_Idle);
}

void ABasePlayerCharacter::AnimNotify_HitEnd()
{
    SetPlayerState(EPawnState::E_Idle);
}