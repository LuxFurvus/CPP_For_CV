// Copyright Epic Games, Inc. All Rights Reserved.

#include "C_PlayerCharacter.h"
#include "Engine/EngineTypes.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
//////////////////////////////
#include "C_BuildGround.h"
#include "I_MarkBaseConnect.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


//////////////////////////////////////////////
/////////////+++COMMON MEMBERS+++///////////////////////
/////////////////////////////////////////////////////////////////////


AC_PlayerCharacter::AC_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(60.f, 60.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 750.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->AirControl = 10.0f;
	GetCharacterMovement()->MaxWalkSpeed = 750.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 5000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 5000.0f;
	GetCharacterMovement()->SetWalkableFloorAngle(90.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	SphereBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereBody"));
	SphereBody->SetupAttachment(GetCapsuleComponent());
	SphereBody->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f));
	SphereBody->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	SphereBody->CastShadow = false;

	TurretProjection = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretProjection"));
	TurretProjection->SetupAttachment(RootComponent);
	TurretProjection->SetWorldScale3D(FVector(7.0f, 7.0f, 7.0f));
	TurretProjection->SetVisibility(false);

	SetReplicates(true);
}

void AC_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController) return;

	GetWorldTimerManager().SetTimer(BuildCooldownTimer, [this]() { CheckBuildAbility(); }, 0.5f, true);
	GetWorldTimerManager().PauseTimer(BuildCooldownTimer);

	FTimerHandle LookForPlaceTimer;
	GetWorldTimerManager().SetTimer(LookForPlaceTimer, [this]() { LookForBuildPlace(); }, 0.01f, true);

	NonBuildMat = SphereBody->GetStaticMesh()->GetMaterial(0);
	MakeMat_SERVER();

	WBP_BuildMenu = CreateWidget(PlayerController, BuildScreenClass);
	WBP_BuildMenu->SetVisibility(ESlateVisibility::Collapsed);
	WBP_BuildMenu->AddToPlayerScreen();

	if (!TurretProjection) return;
	BuildColor = TurretProjection->CreateDynamicMaterialInstance(0);
	if (!BuildColor) return;
	TurretProjection->SetMaterial(0, BuildColor);
	SelectTurretColor("Violet");

	///////INPUT
	using EILPS = UEnhancedInputLocalPlayerSubsystem;
	EILPS* Subsystem = ULocalPlayer::GetSubsystem<EILPS>(PlayerController->GetLocalPlayer());
	if (!Subsystem) return;
	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void AC_PlayerCharacter::MakeMat_SERVER_Implementation()
{
	NonBuildMat = SphereBody->GetStaticMesh()->GetMaterial(0);
	MakeMat_MULTI();
}
void AC_PlayerCharacter::MakeMat_MULTI_Implementation()
{
	NonBuildMat = SphereBody->GetStaticMesh()->GetMaterial(0);
}

void AC_PlayerCharacter::Tick(float DeltaTime) {}

void AC_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_PlayerCharacter, TurretTraits); 
	DOREPLIFETIME(AC_PlayerCharacter, CanBuildTurret);
}

////////////////////////// INPUT

void AC_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent) {
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		return;
	}
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AC_PlayerCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AC_PlayerCharacter::Look);
}

void AC_PlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AC_PlayerCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

//////////////////////////////////////////////
/////////////+++TASK 1+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////

void AC_PlayerCharacter::SelectTurretColor(const FName& Color)
{
	TurrColor = Color;
	
	if (!BuildColor || !DT_TurretTraits.DataTable) return;

	FName RowName{ DT_TurretTraits.RowName };
	F_TurretTraits* Traits = DT_TurretTraits.DataTable->FindRow<F_TurretTraits>(Color, "");
	if (!Traits) return;
	TurretTraits = *Traits;

	BuildColor->SetVectorParameterValue("COLOR", Traits->Color);
	IsTurretSelected = true;
}

void AC_PlayerCharacter::LookForBuildPlace()
{
	FRotator CameraRot{ FollowCamera->GetComponentRotation() };
	FVector LineStart{ FollowCamera->GetComponentLocation() };
	FVector LineEnd{ UKismetMathLibrary::GetForwardVector(CameraRot) * BuildDistance + LineStart };

	ECollisionChannel TraceChannel{ ECC_Visibility };
	FCollisionQueryParams LineParams;

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, LineStart, LineEnd, TraceChannel, LineParams);

	AC_BuildGround* BuildGround { Cast<AC_BuildGround>(Hit.GetActor()) };
	if (!BuildGround) return;

	FVector ProjLoc{ Hit.ImpactPoint };

	FVector ShereLoc1{ ProjLoc };
	FVector ShereLoc2{ ProjLoc * 1.001f};

	FCollisionShape MySphere = FCollisionShape::MakeSphere(350.0f);
	TArray<FHitResult> OutResults;
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActor(BuildGround);
	GetWorld()->SweepMultiByChannel(OutResults, ShereLoc1, ShereLoc2, FQuat::Identity, TraceChannel, MySphere, SphereParams);

	FVector ProjHiddenPoint {GetActorLocation() * -5000};

	if (OutResults.Num() != 0)
	{
		TurretProjection->SetWorldLocation(ProjHiddenPoint);
		HasPlaceToBuild = false;
		return;
	}
	TurretProjection->SetWorldLocation(ProjLoc);
	PlaceToBuild = ProjLoc;
	HasPlaceToBuild = true;
	return;
}

//////////////////////////////////////////////
/////////////+++TASK 2+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////


/////////////+++2.b (Building Mode)

void AC_PlayerCharacter::OpenBuildMode()
{
	if (IsInBuildingMode)	{
		TurretProjection->SetVisibility(false);
		WBP_BuildMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{	
		TurretProjection->SetVisibility(true);
		WBP_BuildMenu->SetVisibility(ESlateVisibility::Visible);
	}
	OpenBuildMode_SERVER();
}

void AC_PlayerCharacter::OpenBuildMode_SERVER_Implementation()
{
	if (IsDead || !CanBuild || !PlayerBase) return;

	OpenBuildMode_MULTI();
}

void AC_PlayerCharacter::OpenBuildMode_MULTI_Implementation()
{
	//if (IsDead) return;

	if (!IsInBuildingMode) { NonBuildMat = SphereBody->GetMaterial(0); }
	UMaterialInterface* NewMat = (IsInBuildingMode) ? NonBuildMat : BuildMat;
	SphereBody->SetMaterial(0, NewMat);
	IsInBuildingMode = !IsInBuildingMode;
}

/////////////+++2.c (Building Cooldown)

void AC_PlayerCharacter::SetUnbuildable_SERVER_Implementation(bool CanBuildNow)
{
	LastBuildTime = GetGameTimeSinceCreation();
	CanBuild = CanBuildNow;
	SetUnbuildable_MULTI(CanBuildNow);
}

void AC_PlayerCharacter::SetUnbuildable_MULTI_Implementation(bool CanBuildNow)
{
	LastBuildTime = GetGameTimeSinceCreation();
	CanBuild = CanBuildNow;
	if (!CanBuildNow)
	{
		NonBuildShape = SphereBody->GetStaticMesh();
		SphereBody->SetWorldScale3D(FVector(5.0f, 5.0f, 5.0f));
		SphereBody->SetStaticMesh(BuildShape);
		SphereBody->SetMaterial(0, BuildMat);
		return;
	}
	else
	{
		SphereBody->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
		SphereBody->SetStaticMesh(NonBuildShape);
		SphereBody->SetMaterial(0, NonBuildMat);
		return;
	}
}


//////////////////////////////////////////////
/////////////+++TASK 3+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////

void AC_PlayerCharacter::BuildTurret(const FDataTableRowHandle& DT)
{
	FName RowName{ DT.RowName };
	F_TurretTraits* TurrTraits = DT.DataTable->FindRow<F_TurretTraits>(TurrColor, "");
	if (!TurrTraits) return;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(TurrTraits->Price) + "!!!!", true, false, FLinearColor(FColor::Black), 3.0f);

	II_MarkBaseConnect* BaseInterface{ Cast<II_MarkBaseConnect>(PlayerBase) };
	if (!BaseInterface) return;

	bool HasMoney{ BaseInterface->GetBaseMoney() > TurrTraits->Price };

	CanBuildTurret = IsInBuildingMode && !IsDead && CanBuild && HasMoney;
}

void AC_PlayerCharacter::BuildTurret_SERVER_Implementation(const FDataTableRowHandle& DT)
{	
	FName RowName{ DT.RowName };
	F_TurretTraits* TurrTraits = DT.DataTable->FindRow<F_TurretTraits>(TurrColor, "");
	if (!TurrTraits) return;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(TurrTraits->Price) + "!!!!", true, false, FLinearColor(FColor::Black), 3.0f);

	II_MarkBaseConnect* BaseInterface{ Cast<II_MarkBaseConnect>(PlayerBase) };
	if (!BaseInterface) return;

	bool HasMoney{ BaseInterface->GetBaseMoney() > TurrTraits->Price };

	CanBuildTurret = IsInBuildingMode && !IsDead && CanBuild && HasMoney;

	if (!CanBuildTurret) return;

	if (!BuildCooldownTimer.IsValid()) return;
	GetWorldTimerManager().UnPauseTimer(BuildCooldownTimer);
	IsInBuildingMode = false;
	BuildTurret_MULTI();

	OpenBuildMode_SERVER();
	SetUnbuildable_SERVER(false);

	BaseInterface->ChangeMoneyOfBase(-TurrTraits->Price);

	return;
	//////////////////////////////////////////
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(TurretTraits.Price) + "!!!!", true, false, FLinearColor(FColor::Black), 3.0f);

	//if (!this) return;

	//if (!TurretClass) return;
	//UKismetSystemLibrary::PrintString(GetWorld(), "TurretClass", true, false, FLinearColor(FColor::Black), 3.0f);

	//FTransform TurrTransf;
	//TurrTransf.SetLocation(Place);
	//FActorSpawnParameters Params;

	//AActor* Spawn = GetWorld()->SpawnActor<AActor>(TurretClass->StaticClass(), TurrTransf, Params);

	//AC_Turret* Turret{ Cast<AC_Turret>(Spawn) };
	//if (!Turret) return;

	//Turret->SetTurretTraits(*TurrTraits);
	//Turret->SetOwner(TurretOwner);
}

void AC_PlayerCharacter::BuildTurret_MULTI_Implementation()
{
	IsInBuildingMode = false;
	SphereBody->SetMaterial(0, NonBuildMat);
	GetWorldTimerManager().UnPauseTimer(BuildCooldownTimer);
}

void AC_PlayerCharacter::KillTurret_SERVER_Implementation(int32 TurretPrice)
{
	if (IsDead || !PlayerBase) return;

	II_MarkBaseConnect* MarkBaseInterface{ Cast<II_MarkBaseConnect>(PlayerBase) };
	if (!MarkBaseInterface) return;
	MarkBaseInterface->ChangeMoneyOfBase(TurretPrice);
}


//////////////////////////////////////////////
/////////////+++TASK 4+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////

void AC_PlayerCharacter::SetPlayerBaseLink_CLIENT_Implementation()
{
	if (PlayerBase) return;

	FHitResult Hit;

	FRotator CameraRot{ FollowCamera->GetComponentRotation() };
	FVector LineStart{ GetActorLocation() };
	FVector LineEnd{ UKismetMathLibrary::GetForwardVector(CameraRot) * 1000 + LineStart };

	ECollisionChannel TraceChannel{ ECC_Visibility };
	FCollisionQueryParams CollisionParams;

	GetWorld()->LineTraceSingleByChannel(Hit, LineStart, LineEnd, TraceChannel, CollisionParams);

	AActor* HitActor{ Hit.GetActor() };
	if (!HitActor) return;

	II_MarkBaseConnect* MarkInterface{ Cast<II_MarkBaseConnect>(HitActor) };
	if (!MarkInterface) return;

	bool IsPlayerBase{ MarkInterface->CheckForBase() };

	if (!IsPlayerBase || HitActor->GetOwner()) return;

	HitActor->SetOwner(this);

	MarkInterface->ClaimBase();

	APlayerController* NewBaseOwner{ Cast<APlayerController>(GetController()) };

	if (!NewBaseOwner) return;

	PlayerScreen = CreateWidget(NewBaseOwner, PlayerScreenClass);

	UGameplayStatics::PlaySound2D(GetWorld(), ClaimBaseSound, 1.0f, 1.0f);

	SetPlayerBaseLink_SERVER(HitActor);
}

void AC_PlayerCharacter::SetPlayerBaseLink_SERVER_Implementation(AActor* PlayerBaseActor)
{
	PlayerBase = PlayerBaseActor;
	PlayerBase->SetOwner(this);

	II_MarkBaseConnect* MarkInterface{ Cast<II_MarkBaseConnect>(PlayerBase) };
	if (!MarkInterface) return;
	MarkInterface->ClaimBase();

	SetPlayerBaseLink_MULTI(PlayerBaseActor);
}

void AC_PlayerCharacter::SetPlayerBaseLink_MULTI_Implementation(AActor* PlayerBaseActor)
{
	PlayerBase = PlayerBaseActor;
	PlayerBase->SetOwner(this);

	II_MarkBaseConnect* MarkInterface{ Cast<II_MarkBaseConnect>(PlayerBase) };
	if (!MarkInterface) return;
	MarkInterface->ClaimBase();
}

void AC_PlayerCharacter::TEST_ChangeBaseHealth_SERVER_Implementation(float AddHealth)
{
	if (IsDead || !PlayerBase) return;

	II_MarkBaseConnect* MarkInterface{ Cast<II_MarkBaseConnect>(PlayerBase) };
	if (!MarkInterface) return;
	MarkInterface->ChangeHealthOfBase(AddHealth);
}

void AC_PlayerCharacter::MakeEndScreen() {
	if (PlayerScreen)
	{ 
		PlayerScreen->AddToPlayerScreen();
	}

	if (BuildCooldownTimer.IsValid()) 
	{
		GetWorldTimerManager().PauseTimer(BuildCooldownTimer);
	}

	MakeEndScreen_SERVER(); 

	////////We can disable player movement after defeat
	//UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	//if (!EnhancedInputComponent) return;
	//EnhancedInputComponent->ClearActionBindings();
	//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AC_PlayerCharacter::Look);
}

void AC_PlayerCharacter::MakeEndScreen_SERVER_Implementation() {
	SphereBody->SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f));
	MakeEndScreen_MULTI();
}

void AC_PlayerCharacter::MakeEndScreen_MULTI_Implementation()
{
	SphereBody->SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f));
}

void AC_PlayerCharacter::CheckBuildAbility()
{
	bool IsCooldownLeft{ (GetGameTimeSinceCreation() - LastBuildTime) > BuildCooldownTime };
	bool IsFirsTimeBuild{ LastBuildTime == 0 };
	bool ToAllow{ IsFirsTimeBuild || IsCooldownLeft };

	UKismetSystemLibrary::PrintString(
		GetWorld(),
		FString::SanitizeFloat(GetGameTimeSinceCreation() - LastBuildTime),
		true, false, FLinearColor(FColor::Black), 1.0f);

	if (!ToAllow) return;

	CanBuild = true;
	GetWorldTimerManager().PauseTimer(BuildCooldownTimer);
	SetUnbuildable_SERVER(true);
}

//////////////////////////////////////////////////////////////////////////
//////////FOR_BP_TESTING_ONLY
void AC_PlayerCharacter::MakeEndScreenBP_Implementation() {}

//////////////////////////////////////////////////////////////////////////
//UKismetSystemLibrary::PrintString(GetWorld(), "ChangeBaseHealth", true, false, FLinearColor(FColor::Black), 3.0f);

//bool IsPlayerBase0{ HitActor->GetClass()->ImplementsInterface(UI_MarkBaseConnect::StaticClass()) };
//if (!IsPlayerBase0) return;
//Cast<II_MarkBaseConnect>(Hit.GetActor())->CheckForBase();
//II_MarkBaseConnect::Execute_CheckForBaseBP(Hit.GetActor());
//bool IsPlayerBase{ MarkInter->Execute_CheckForBaseBP(HitActor) };
