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

ABasePlayerCharacter::ABasePlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

    CurrentWeapon = nullptr;

    WeaponSocketName = FName("WeaponSocket");
}

void ABasePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
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

    if (CurrentWeapon)
    {
        PlayAttackMontage();
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("No weapon equipped"));
        }
    }
}

void ABasePlayerCharacter::PlayAttackMontage()
{
    if (AttackMontage)
    {
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
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No AttackMontage assigned"));
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

    // route the input
    DoLook(LookAxisVector.X, LookAxisVector.Y);

    
}

void ABasePlayerCharacter::DoLook(float Yaw, float Pitch)
{
    {
        if (GetController() != nullptr)
        {
            // add yaw and pitch input to controller
            AddControllerYawInput(Yaw);
            AddControllerPitchInput(Pitch);
        }
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