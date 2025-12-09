#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UInputMappingContext;
class UMainHUD;

UCLASS()
class UNI_PROJECT_FINAL_API ABasePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
    TSubclassOf<UMainHUD> HUDWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UMainHUD* CurrentHUD;

protected:
    virtual void BeginPlay() override;

    void CreateHUD();
};