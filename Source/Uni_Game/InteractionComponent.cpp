#include "InteractionComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

AActor* UInteractionComponent::CheckForInteractables()
{
    if (!OwnerCharacter) return nullptr;

    FVector Start = OwnerCharacter->GetActorLocation();

    FVector Forward = OwnerCharacter->GetActorForwardVector();

    FVector End = Start + (Forward * InteractionDistance);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        bHit ? FColor::Green : FColor::Red,
        false,
        1.0f,
        0,
        2.0f
    );

    return bHit ? HitResult.GetActor() : nullptr;
}