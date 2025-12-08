#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InteractionComponent.h" 
#include "InteractionInterface.h"  
#include "Weapon.h"
#include "CombatInterface.h"
#include "BasePlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EPlayerState : uint8  
{
    E_Idle,
    E_Combat,
    E_Hit,
    E_Occupied,
    E_Dead
};

UCLASS()
class UNI_PROJECT_FINAL_API ABasePlayerCharacter : public ABaseCharacter, public ICombatInterface
{
    GENERATED_BODY()

public:
    ABasePlayerCharacter();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformWeaponAttackTrace();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool CanPerformAttack() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PlayAttackMontage();

    UFUNCTION()
    void AnimNotify_AttackTrace();

    UFUNCTION()
    void AnimNotify_AttackEnd();

    UFUNCTION()
    void AnimNotify_HitEnd();

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAttributesComponent* AttributesComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName WeaponSocketName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
    EPlayerState CombatState;  

    UFUNCTION(BlueprintCallable, Category = "State")
    void SetPlayerState(EPlayerState NewState);

    void EquipWeapon(AWeapon* NewWeapon);

    virtual void BeginPlay() override;

    void Move(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void Interact(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void DoLook(float Yaw, float Pitch);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* HitMontage;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PlayHitMontage();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void GetHit(float DamageAmount);
    virtual void GetHit_Implementation(float DamageAmount) override;

    UFUNCTION()
    void HandleDeath();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* DeathMontage;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PlayDeathMontage();

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};