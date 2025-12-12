#include "BTTask_AttackTarget.h"
#include "AIController.h"
#include "BaseEnemyAIController.h"
#include "BaseEnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
    NodeName = "Attack Target";

    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AttackTarget, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseEnemyAIController* AIController = Cast<ABaseEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    ABaseEnemyCharacter* EnemyChar = Cast<ABaseEnemyCharacter>(AIController->GetPawn());
    if (!EnemyChar)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: No target!"));
        return EBTNodeResult::Failed;
    }

    if (!EnemyChar->IsTargetValid(Target))
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Target is not valid (dead)"));
        BlackboardComp->ClearValue(TargetActorKey.SelectedKeyName);
        return EBTNodeResult::Failed;
    }

    float Distance = FVector::Dist(EnemyChar->GetActorLocation(), Target->GetActorLocation());
    if (Distance > AttackRange)
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Target too far (%.1f > %.1f)"), Distance, AttackRange);
        return EBTNodeResult::Failed;
    }

    if (!EnemyChar->CanPerformAttack())
    {
        UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Cannot perform attack (wrong state)"));
        return EBTNodeResult::Failed;
    }

    if (EnemyChar->AttributesComponent)
    {
        float AttackCost = EnemyChar->AttributesComponent->GetStaminaCosts().StaminaCost_Attack;

        if (!EnemyChar->AttributesComponent->CanPayStaminaCost(AttackCost))
        {
            UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Not enough stamina"));
            return EBTNodeResult::Failed;
        }

        EnemyChar->AttributesComponent->PayStamina(AttackCost);
    }

    EnemyChar->StartAttack();

    UE_LOG(LogTemp, Display, TEXT("BTTask_Attack: Attack executed successfully!"));

    return EBTNodeResult::Succeeded;
}