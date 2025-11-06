// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionInterface2.h"

ABasePlayerCharacter::ABasePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void ABasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ABasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::OnMove);

		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::OnLook);

		EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ABasePlayerCharacter::OnInteract);

		EnhancedInput->BindAction(AttackAction, ETriggerEvent::Started, this, &ABasePlayerCharacter::OnAttack);
	}
}

void ABasePlayerCharacter::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);

		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABasePlayerCharacter::OnLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABasePlayerCharacter::OnInteract(const FInputActionValue& Value)
{
	if (!InteractionComponent) return;

	AActor* HitActor = InteractionComponent->CheckForInteractables();

	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Nothing to interact with"));
		return;
	}

	AAWeapon* Weapon = Cast<AAWeapon>(HitActor);
	if (Weapon)
	{
		EquipWeapon(Weapon);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit actor is not a weapon"));
	}
}

void ABasePlayerCharacter::OnAttack(const FInputActionValue& Value)
{
	if (CurrentWeapon)
	{
		UE_LOG(LogTemp, Log, TEXT("Attacking with %s"), *CurrentWeapon->WeaponName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon equipped"));
	}
}

void ABasePlayerCharacter::EquipWeapon(AAWeapon* NewWeapon)
{
	if (!NewWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon: NewWeapon is null"));
		return;
	}

	if (CurrentWeapon)
	{
		UE_LOG(LogTemp, Log, TEXT("Destroying old weapon: %s"), *CurrentWeapon->WeaponName);
		CurrentWeapon->Destroy();
	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon->GetClass()->ImplementsInterface(UInteractionInterface2::StaticClass()))
	{
		IInteractionInterface2::Execute_PickUp(NewWeapon, this);
		UE_LOG(LogTemp, Log, TEXT("Weapon equipped: %s"), *CurrentWeapon->WeaponName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon doesn't implement IInteractionInterface2"));
	}
}