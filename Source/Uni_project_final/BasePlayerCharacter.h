#pragma once


#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InteractionComponent.h" 
#include "InteractionInterface.h"  
#include "Weapon.h"
#include "BasePlayerCharacter.generated.h"


class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class UNI_PROJECT_FINAL_API ABasePlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ABasePlayerCharacter();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformWeaponAttackTrace();

protected:

	
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* AttackAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInteractionComponent* InteractionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName WeaponSocketName;

    void EquipWeapon(AWeapon* NewWeapon);


    virtual void BeginPlay() override;

    void Move(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void Interact(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void DoLook(float Yaw, float Pitch);


	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    void PlayAttackMontage();

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
