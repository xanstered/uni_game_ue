#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackTarget.generated.h"


UCLASS()
class UNI_PROJECT_FINAL_API UBTTask_AttackTarget : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_AttackTarget();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "Combat")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 200.0f;
};