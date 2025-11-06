#include "AItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

AAItem::AAItem()
{
    PrimaryActorTick.bCanEverTick = false;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;

    ItemMesh->SetSimulatePhysics(false);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetSphereRadius(150.0f);
    InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AAItem::BeginPlay()
{
    Super::BeginPlay();
}

void AAItem::PickUp_Implementation(AActor* PlayerCharacter)
{
    UE_LOG(LogTemp, Warning, TEXT("AAItem::PickUp called on %s"), *GetName());
}