#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
    WeaponCollision->SetupAttachment(RootComponent);
    WeaponCollision->SetBoxExtent(FVector(50.0f, 10.0f, 10.0f)); // Dostosuj rozmiar do swojej broni
    WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Domyœlnie wy³¹czona
    WeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlap);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

void AWeapon::PickUp_Implementation(AActor* Interactor)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
            FString::Printf(TEXT("picked up weapon: %s"), *WeaponName));
    }

    if (ItemMesh)
    {
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

   // SetActorHiddenInGame(true);
}

void AWeapon::EquipWeapon(AActor* NewOwner, USceneComponent* AttachSocket)
{
    if (!NewOwner || !AttachSocket)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("EquipWeapon: Invalid params"));
        }
        return;
    }

    SetActorHiddenInGame(false);
    SetActorEnableCollision(false);

    FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
    AttachToComponent(AttachSocket, AttachRules);

    SetOwner(NewOwner);

    if (ItemMesh)
    {
        ItemMesh->SetSimulatePhysics(false);
        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        ItemMesh->SetVisibility(true); 
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            FString::Printf(TEXT("Weapon VISIBLE and attached")));
    }
}

void AWeapon::ActivateWeaponCollision()
{
    if (WeaponCollision)
    {
        WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Weapon collision ACTIVATED"));
        }
    }
}

void AWeapon::DeactivateWeaponCollision()
{
    if (WeaponCollision)
    {
        WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Weapon collision DEACTIVATED"));
        }
    }
}

void AWeapon::OnAimNotify()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("AimNotify received"));
    }
}

void AWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
            FString::Printf(TEXT("HIT: %s"), *OtherActor->GetName()));
    }

}