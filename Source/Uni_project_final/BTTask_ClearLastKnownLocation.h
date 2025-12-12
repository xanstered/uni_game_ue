#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearLastKnownLocation.generated.h"


UCLASS()
class UNI_PROJECT_FINAL_API UBTTask_ClearLastKnownLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_ClearLastKnownLocation();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastKnownLocationKey;
};