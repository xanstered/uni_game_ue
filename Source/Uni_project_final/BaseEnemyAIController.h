#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyCharacter.h"
#include "AttributesComponent.h"
#include "BaseEnemyAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTree;
class UAISenseConfig_Sight;
class UAttributesComponent;
class ABaseEnemyCharacter;
class UAIPerceptionComponent;
enum class EPawnState : uint8;
struct FAIStimulus;

UCLASS()
class UNI_PROJECT_FINAL_API ABaseEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    ABaseEnemyAIController();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* AIPerceptionComponent;

    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void RequestAttack();

    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    bool IsInAttackRange() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackRange = 200.0f;

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION()
    void UpdatePawnStateInBlackboard(EPawnState NewState);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void InitializeBlackboard(UBlackboardData* BlackboardData);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAISenseConfig_Sight* SightConfig;

    void ConfigurePerception();

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION()
    void HandleEnemyDeath();
};