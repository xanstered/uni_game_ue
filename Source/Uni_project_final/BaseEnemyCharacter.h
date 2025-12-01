
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

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UAttributesComponent* AttributesComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    EPawnState PawnState; 

    void SetPawnState(EPawnState NewState);
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void GetHit(float DamageAmount);
    virtual void GetHit_Implementation(float DamageAmount) override;

    UFUNCTION() 
        void HandleDeath(); 

        virtual void BeginPlay() override;
};