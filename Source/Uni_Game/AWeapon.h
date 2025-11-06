#pragma once

#include "CoreMinimal.h"
#include "AItem.h"  
#include "AWeapon.generated.h"

UCLASS()
class UNI_GAME_API AAWeapon : public AAItem
{
    GENERATED_BODY()

public:
    AAWeapon();

    // W³aœciwoœci broni
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    FString WeaponName = TEXT("Default Weapon");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float Damage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float AttackRange = 150.0f;

    // Override funkcji PickUp
    virtual void PickUp_Implementation(AActor* PlayerCharacter) override;

protected:
    virtual void BeginPlay() override;
};