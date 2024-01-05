// Copyright Epic Games, Inc. All Rights Reserved.

#include "cavegenrationCharacter.h"
#include "cavegenrationProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AcavegenrationCharacter

AcavegenrationCharacter::AcavegenrationCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	FlashLight->Intensity = 12000;
	FlashLight->InnerConeAngle = 0;
	FlashLight->OuterConeAngle = 40;
	FlashLight->AttenuationRadius = 800;
	FlashLight->SetupAttachment(FirstPersonCameraComponent);
}

void AcavegenrationCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void AcavegenrationCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsGrappling) {
		FVector direction{ (GrappleLocation - GetActorLocation()).GetSafeNormal() };
		FVector NewLocation = GetActorLocation() + direction * (DeltaTime * MovementSpeed);
		SetActorLocation(NewLocation);
	}

	FRotator CameraRotation = FirstPersonCameraComponent->GetComponentRotation();
	FlashLight->SetWorldRotation(CameraRotation);
}

//////////////////////////////////////////////////////////////////////////// Input

void AcavegenrationCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AcavegenrationCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AcavegenrationCharacter::Look);

		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Triggered, this, &AcavegenrationCharacter::StartGrapple);
		EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Completed, this, &AcavegenrationCharacter::StopGrapple);
	}
}


void AcavegenrationCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AcavegenrationCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AcavegenrationCharacter::StartGrapple()
{
	if (!IsGrappling)
	{
		FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
		FVector CameraForwardVector = FirstPersonCameraComponent->GetForwardVector();

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		FVector GrappleEnd = CameraLocation + CameraForwardVector * GrappleMaxDistance;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, GrappleEnd, ECC_Visibility, CollisionParams))
		{
			// Hook something, handle accordingly (e.g., attach the character to the hit result)
			IsGrappling = true;
			// Implement your grapple logic here.
			GrappleLocation = HitResult.ImpactPoint;
		}
	}
}

void AcavegenrationCharacter::StopGrapple()
{
	IsGrappling = false;
	GrappleLocation = GetActorLocation();
}

void AcavegenrationCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AcavegenrationCharacter::GetHasRifle()
{
	return bHasRifle;
}