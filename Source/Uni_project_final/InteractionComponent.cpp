#include "InteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Cyan, TEXT("TICK WORKS"));
    }

    GetInteractableActor();
}

AActor* UInteractionComponent::GetInteractableActor()
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("debug: No Owner"));
        return nullptr;
    }

    FVector Start = Owner->GetActorLocation();
    FVector Forward = Owner->GetActorForwardVector();
    FVector End = Start + (Forward * InteractionRange);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(50.0f), 
        Params
    );

    DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 3.0f);
    DrawDebugSphere(GetWorld(), End, 50.0f, 8, bHit ? FColor::Green : FColor::Red, false, 0.1f);

    if (bHit && HitResult.GetActor())
    {
        DrawDebugSphere(GetWorld(), HitResult.Location, 30.0f, 12, FColor::Yellow, false, 0.1f);
        return HitResult.GetActor();
    }

    return nullptr;
}