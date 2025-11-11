#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UCLASS()
class UNI_PROJECT_FINAL_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    AWeapon();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FString WeaponName = "default Weapon";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 10.0f;

    virtual void PickUp_Implementation(AActor* Interactor) override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipWeapon(AActor* NewOwner, USceneComponent* AttachSocket);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ActivateWeaponCollision();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void DeactivateWeaponCollision();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void OnAimNotify();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class UBoxComponent* WeaponCollision;

private:
    UFUNCTION()
    void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};