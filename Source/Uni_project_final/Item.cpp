#include "Item.h"
#include "Components/StaticMeshComponent.h"

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = false;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;

    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
    ItemMesh->SetSimulatePhysics(true);
}

void AItem::BeginPlay()
{
    Super::BeginPlay();
}

void AItem::PickUp_Implementation(AActor* Interactor)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
            FString::Printf(TEXT("picked up by: %s"), *Interactor->GetName()));
    }

    Destroy();
}