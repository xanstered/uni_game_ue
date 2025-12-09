#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

UCLASS()
class UNI_PROJECT_FINAL_API UMainHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdateStamina(float CurrentStamina, float MaxStamina);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void UpdatePawnState(const FString& StateName);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void ShowLowStaminaWarning();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void BindToAttributesComponent(class UAttributesComponent* AttributesComp);
};