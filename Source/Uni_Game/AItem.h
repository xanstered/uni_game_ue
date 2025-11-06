#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "AItem.generated.h"

UCLASS()
class UNI_GAME_API AAItem : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    AAItem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ItemMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* InteractionSphere;

    virtual void PickUp_Implementation(AActor* PlayerCharacter);

protected:
    virtual void BeginPlay() override;
};