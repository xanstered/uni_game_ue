#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InteractionComponent.h" 
#include "InteractionInterface.h"  
#include "Weapon.h"
#include "CombatInterface.h"
#include "PawnStateEnum.h"  
#include "BasePlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UMainHUD;


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

    UFUNCTION(BlueprintPure, Category = "State")
    EPawnState GetPlayerState() const { return CombatState; } 

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* AttackAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* InteractAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* SprintAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInteractionComponent* InteractionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAttributesComponent* AttributesComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName WeaponSocketName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
    EPawnState CombatState; 

    UFUNCTION(BlueprintCallable, Category = "State")
    void SetPlayerState(EPawnState NewState); 

    void EquipWeapon(AWeapon* NewWeapon);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void Move(const FInputActionValue& Value);
    void Attack(const FInputActionValue& Value);
    void Interact(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void DoLook(float Yaw, float Pitch);

    void Jump(const FInputActionValue& Value);
    void StartSprint(const FInputActionValue& Value);
    void StopSprint(const FInputActionValue& Value);

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

    void SetupHUD();
    void UpdateHUDState();

    UPROPERTY()
    UMainHUD* PlayerHUD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeed = 1000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsSprinting = false;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};