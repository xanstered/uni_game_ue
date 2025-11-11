#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Item.generated.h"

UCLASS()
class UNI_PROJECT_FINAL_API AItem : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    AItem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ItemMesh;

    virtual void PickUp_Implementation(AActor* Interactor) override;

protected:
    virtual void BeginPlay() override;
};