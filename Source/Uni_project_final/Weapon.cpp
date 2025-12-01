#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "BaseEnemyCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionQueryParams.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "Math/Quat.h"
#include "CombatInterface.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
    WeaponCollision->SetupAttachment(RootComponent);
    WeaponCollision->SetBoxExtent(FVector(50.0f, 10.0f, 10.0f)); 
    WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
    WeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlap);

    BoxTraceExtent = FVector(50.0f, 10.0f, 10.0f);

    TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("TraceStart"));
    TraceStart->SetupAttachment(RootComponent);

    TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("TraceEnd"));
    TraceEnd->SetupAttachment(RootComponent);

    TraceStart->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
    TraceEnd->SetRelativeLocation(FVector(150.0f, 0.0f, 0.0f));
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
    USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(AttachSocket);

    if (SkeletalMesh)
    {
        FAttachmentTransformRules AttachRules(
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            false
        );

        AttachToComponent(SkeletalMesh, AttachRules, FName("WeaponSocket"));
    }

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
            FString::Printf(TEXT("Weapon attached to WeaponSocket")));
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
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || Cast<ABaseEnemyCharacter>(OtherActor) == nullptr) 
    {
        return;
    }

    //PerformWeaponTrace();

    //DeactivateWeaponCollision();


    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
            FString::Printf(TEXT("OVERLAP DETECTED with: %s"), *OtherActor->GetName()));
    }

}

void AWeapon::PerformWeaponTrace()
{
    if (!TraceStart || !TraceEnd) return;

    FVector StartLocation = TraceStart->GetComponentLocation();
    FVector EndLocation = TraceEnd->GetComponentLocation();
    FRotator Orientation = TraceStart->GetComponentRotation();

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.bTraceComplex = true;

    if (GetOwner())
    {
        Params.AddIgnoredActor(GetOwner());
    }

    bool bHit = UKismetSystemLibrary::BoxTraceSingle(
        GetWorld(),
        StartLocation,
        EndLocation,
        BoxTraceExtent,
        Orientation,
        ETraceTypeQuery::TraceTypeQuery1,
        false,
        TArray<AActor*>(),
        EDrawDebugTrace::ForDuration,    
        HitResult,
        true
    );

#if ENABLE_DRAW_DEBUG
    FColor DebugColor = bHit ? FColor::Green : FColor::Red;

    DrawDebugBox(
        GetWorld(),
        (StartLocation + EndLocation) / 2.0f,
        BoxTraceExtent,                   
        Orientation.Quaternion(),
        DebugColor,
        false,                               
        2.0f,                                 
        0                                    
    );
#endif


    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        FVector ImpactLocation = HitResult.ImpactPoint;

        ICombatInterface* CombatTarget = Cast<ICombatInterface>(HitActor);
        if (CombatTarget)
        {
            CombatTarget->Execute_GetHit(HitActor, Damage); 

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                    FString::Printf(TEXT("Damage dealt to: %s"), *HitActor->GetName()));
            }
        }

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
                FString::Printf(TEXT("BOX TRACE HIT: %s at location: %s"),
                    *HitActor->GetName(), *ImpactLocation.ToString()));
        }
    }
}

