#include "BasePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "MainHUD.h"
#include "Blueprint/UserWidget.h"

void ABasePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    CreateHUD();
}

void ABasePlayerController::CreateHUD()
{
    if (HUDWidgetClass)
    {
        CurrentHUD = CreateWidget<UMainHUD>(this, HUDWidgetClass);
        if (CurrentHUD)
        {
            CurrentHUD->AddToViewport();

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                    TEXT("HUD created and added to viewport"));
            }
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
                TEXT("ERROR: HUDWidgetClass not set in PlayerController!"));
        }
    }
}