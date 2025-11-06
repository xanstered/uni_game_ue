#include "AWeapon.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

AAWeapon::AAWeapon()
{
}

void AAWeapon::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("Weapon spawned: %s"), *WeaponName);
}

void AAWeapon::PickUp_Implementation(AActor* PlayerCharacter)
{
    ACharacter* Character = Cast<ACharacter>(PlayerCharacter);
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("PickUp: PlayerCharacter is not a Character"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Picking up weapon: %s"), *WeaponName);

    FAttachmentTransformRules AttachRules(
        EAttachmentRule::SnapToTarget,  
        EAttachmentRule::SnapToTarget, 
        EAttachmentRule::KeepWorld,     
        false                            
    );

    bool bAttached = AttachToComponent(
        Character->GetMesh(),
        AttachRules,
        FName("WeaponSocket")  
    );

    if (bAttached)
    {
        UE_LOG(LogTemp, Log, TEXT("Weapon attached successfully"));

        ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        ItemMesh->SetSimulatePhysics(false);

        InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to attach weapon! Check socket name"));
    }
}