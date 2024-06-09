// Copyright Epic Games, Inc. All Rights Reserved.

#include "Atlant_A_GameMode.h"
#include "Atlant_A_PlayerController.h"
#include "Atlant_A_Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

AAtlant_A_GameMode::AAtlant_A_GameMode()
{
	DefaultPawnClass = nullptr;
	GameStateClass = AAtlant_A_GameState::StaticClass();
	PlayerControllerClass = AAtlant_A_PlayerController::StaticClass();
}

void AAtlant_A_GameMode::BeginPlay()
{
	Super::BeginPlay();
	MainItemType = static_cast<E_ItemTypes>(UKismetMathLibrary::RandomIntegerInRange(1, 3));
}
