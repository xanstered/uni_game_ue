#include "BaseEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"    
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"    
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "BaseEnemyCharacter.h" 
#include "BasePlayerCharacter.h" 
#include "AttributesComponent.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Components/PrimitiveComponent.h" 
#include "AI/NavigationSystemBase.h" 

const FName TargetActorKey("TargetActor");
const FName LastKnownPlayerLocationKey("LastKnownPlayerLocation");
const FName PawnStateKey("EPawnState");
const FName IsDeadKey("IsDead");

ABaseEnemyAIController::ABaseEnemyAIController()
{
    if (!AIPerceptionComponent)
    {
        AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    }

    ConfigurePerception();
}

void ABaseEnemyAIController::ConfigurePerception()
{
    if (!AIPerceptionComponent)
    {
        return;
    }

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    if (SightConfig)
    {
        SightConfig->SightRadius = 1500.0f;
        SightConfig->LoseSightRadius = 1800.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->SetMaxAge(5.0f);
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::OnTargetPerceptionUpdated);
    }
}

void ABaseEnemyAIController::UpdatePawnStateInBlackboard(EPawnState NewState)
{
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsEnum(PawnStateKey, (uint8)NewState);
    }
}

void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset && BlackboardComponent)
    {
        if (!BlackboardComponent->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset))
        {
            UE_LOG(LogTemp, Error, TEXT("AIController: Failed to initialize Blackboard!"));
            return;
        }

        RunBehaviorTree(BehaviorTreeAsset);

        ABaseEnemyCharacter* EnemyChar = Cast<ABaseEnemyCharacter>(InPawn);
        if (EnemyChar)
        {
            BlackboardComponent->SetValueAsEnum(PawnStateKey, (uint8)EnemyChar->PawnState);

            EnemyChar->OnStateChanged.AddDynamic(this, &ABaseEnemyAIController::UpdatePawnStateInBlackboard);

            if (EnemyChar->AttributesComponent)
            {
                EnemyChar->AttributesComponent->OnDeathDelegate.AddDynamic(this, &ABaseEnemyAIController::HandleEnemyDeath);
            }
        }
    }
}

void ABaseEnemyAIController::InitializeBlackboard(UBlackboardData* BlackboardData)
{
    if (BlackboardData && BlackboardComponent)
    {
        BlackboardComponent->InitializeBlackboard(*BlackboardData);
    }
}

void ABaseEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    ABasePlayerCharacter* PlayerCharacter = Cast<ABasePlayerCharacter>(Actor);
    if (!PlayerCharacter) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        BlackboardComponent->SetValueAsObject(TargetActorKey, Actor);

        if (BlackboardComponent->GetValueAsEnum(PawnStateKey) != (uint8)EPawnState::E_Dead)
        {
            BlackboardComponent->SetValueAsEnum(PawnStateKey, (uint8)EPawnState::E_Combat);
        }

        UE_LOG(LogTemp, Warning, TEXT("Gracz wykryty: %s"), *Actor->GetName());
    }
    else
    {
        if (BlackboardComponent->GetValueAsObject(TargetActorKey) == Actor)
        {
            BlackboardComponent->SetValueAsVector(LastKnownPlayerLocationKey, Stimulus.StimulusLocation);
            BlackboardComponent->ClearValue(TargetActorKey);

            UE_LOG(LogTemp, Warning, TEXT("Gracz zgubiony, ostatnia pozycja: %s"), *Stimulus.StimulusLocation.ToString());
        }
    }
}

void ABaseEnemyAIController::HandleEnemyDeath()
{
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsBool(IsDeadKey, true);
        StopMovement(); 
    }
}