// Copyright Epic Games, Inc. All Rights Reserved.

#include "Atlant_A_Character.h"
#include "Atlant_I_ForInteractComp.h"
#include "Atlant_I_FromGameState.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameStateBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AAtlant_A_Character

AAtlant_A_Character::AAtlant_A_Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 150.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InteractComponent = CreateDefaultSubobject<UAtlant_U_InteractComponent>(TEXT("InteractComponent"));
	if (!InteractComponent) return;
	AddOwnedComponent(InteractComponent);
}

void AAtlant_A_Character::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) return;

	//Add Input Mapping
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem) return;

	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	GameState = UGameplayStatics::GetGameState(GetWorld());
	if (!GameState) return;

	GameStateI = Cast<IAtlant_I_FromGameState>(GameState);
	if (!GameStateI) return;

	const TArray<UActorComponent*> InteractCompArr = GetComponentsByInterface(UAtlant_I_ForInteractComp::StaticClass());
	if (InteractCompArr.Num() <= 0) return;
	InteractComp = InteractCompArr[0];
	if (!InteractComp) return;

	InteractCompI = Cast<IAtlant_I_ForInteractComp>(InteractComp);
	if (!InteractCompI) return;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAtlant_A_Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent) return;

	//Jumping
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	//Moving
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAtlant_A_Character::Move);

	//Looking
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAtlant_A_Character::Look);

	// Interact
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AAtlant_A_Character::InteractFunc);

	// Quit
	EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Triggered, this, &AAtlant_A_Character::QuitFunc);
}

void AAtlant_A_Character::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AAtlant_A_Character::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AAtlant_A_Character::InteractFunc(const FInputActionValue& Value)
{
	if (!Controller || !InteractComp || !InteractCompI) return;

	InteractCompI->RunInteraction();
}

void AAtlant_A_Character::QuitFunc(const FInputActionValue& Value)
{
	if (!Controller || !GameState || !GameStateI || !GameStateI->GetIsEnd()) return;

	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}
