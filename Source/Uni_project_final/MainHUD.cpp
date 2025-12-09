#include "MainHUD.h"
#include "AttributesComponent.h"

void UMainHUD::BindToAttributesComponent(UAttributesComponent* AttributesComp)
{
    if (AttributesComp)
    {
        AttributesComp->OnHealthChanged.AddDynamic(this, &UMainHUD::UpdateHealth);
        AttributesComp->OnStaminaChanged.AddDynamic(this, &UMainHUD::UpdateStamina);

        UpdateHealth(AttributesComp->GetHealth(), AttributesComp->GetMaxHealth());
        UpdateStamina(AttributesComp->GetStamina(), AttributesComp->GetMaxStamina());
    }
}