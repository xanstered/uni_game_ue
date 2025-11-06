// BasePlayerController.cpp
#include "BasePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "InteractionComponent.h"
#include "InteractionInterface.h"


void ABasePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Dodaj Input Mapping Context
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (InputMappingContext)
        {
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
}

void ABasePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Rzutowanie na Enhanced Input Component
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Bindowanie akcji
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
        }

        if (AttackAction)
        {
            EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ABasePlayerController::Attack);
        }

        if (InteractAction)
        {
            EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ABasePlayerController::Interact);
        }
    }
}

void ABasePlayerController::Move(const FInputActionValue& Value)
{
    // Pobierz wartoœæ wejœcia (2D Vector dla WASD)
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (APawn* ControlledPawn = GetPawn())
    {
        // Pobierz kierunek kamery
        const FRotator Rotation = GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Oblicz kierunki
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // Dodaj ruch
        ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ABasePlayerController::Attack()
{
    if (ACharacter* Character = Cast<ACharacter>(GetPawn()))
    {
        // Wywo³aj funkcjê ataku na postaci
        UE_LOG(LogTemp, Warning, TEXT("Attack triggered!"));
    }
}

void ABasePlayerController::Interact()
{
    if (ACharacter* Character = Cast<ACharacter>(GetPawn()))
    {
        // Pobierz InteractionComponent z postaci
        UInteractionComponent* InteractComp = Character->FindComponentByClass<UInteractionComponent>();

        if (InteractComp)
        {
            // Pobierz obiekt z którym mo¿emy wchodziæ w interakcjê
            AActor* InteractableActor = InteractComp->GetInteractableInRange();

            if (InteractableActor)
            {
                // SprawdŸ czy implementuje interfejs
                if (InteractableActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
                {
                    // Wywo³aj funkcjê PickUp z interfejsu
                    IInteractionInterface::Execute_PickUp(InteractableActor, Character);
                    UE_LOG(LogTemp, Warning, TEXT("Interacting with: %s"), *InteractableActor->GetName());
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No interactable object in range"));
            }
        }
    }
}