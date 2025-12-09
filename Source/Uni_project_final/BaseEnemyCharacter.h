#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CombatInterface.h" 
#include "AttributesComponent.h"
#include "BaseEnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EPawnState : uint8
{
    E_Idle,
    E_Combat,
    E_Hit,
    E_Occupied,
    E_Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EPawnState, NewState);

UCLASS()
class UNI_PROJECT_FINAL_API ABaseEnemyCharacter : public ABaseCharacter, public ICombatInterface
{
    GENERATED_BODY()

public:
    ABaseEnemyCharacter();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool CanPerformAttack() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopCurrentAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ActivateEnemyWeaponCollision();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void DeactivateEnemyWeaponCollision();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Combat")
    bool IsTargetValid(AActor* Target) const;

    UFUNCTION()
    void AnimNotify_AttackEnd();

    UFUNCTION()
    void AnimNotify_HitEnd();

    UPROPERTY(BlueprintAssignable, Category = "State")
    FOnStateChanged OnStateChanged;

    UFUNCTION(BlueprintPure, Category = "State")
    EPawnState GetPawnState() const { return PawnState; }
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    EPawnState PawnState;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAttributesComponent* AttributesComponent;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    class AWeapon* EnemyWeapon;

    

    void SetPawnState(EPawnState NewState);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* HitMontage;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void GetHit(float DamageAmount);
    virtual void GetHit_Implementation(float DamageAmount) override;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PlayHitMontage();

    UFUNCTION()
    void HandleDeath();
};