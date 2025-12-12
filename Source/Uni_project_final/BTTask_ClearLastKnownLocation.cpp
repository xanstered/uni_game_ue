#include "BTTask_ClearLastKnownLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearLastKnownLocation::UBTTask_ClearLastKnownLocation()
{
    NodeName = "Clear Last Known Location";

    LastKnownLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ClearLastKnownLocation, LastKnownLocationKey));
}

EBTNodeResult::Type UBTTask_ClearLastKnownLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    BlackboardComp->ClearValue(LastKnownLocationKey.SelectedKeyName);

    UE_LOG(LogTemp, Display, TEXT("BTTask: Last Known Location cleared"));

    return EBTNodeResult::Succeeded;
}