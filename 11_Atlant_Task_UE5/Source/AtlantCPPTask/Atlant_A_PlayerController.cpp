// Fill out your copyright notice in the Description page of Project Settings.

#include "Atlant_A_PlayerController.h"
#include "Atlant_W_Menu.h"
#include "Atlant_I_FromGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"

void AAtlant_A_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(ControllerTimer, this, &AAtlant_A_PlayerController::WaitForPlayers, 0.03f, true);
}

void AAtlant_A_PlayerController::WaitForPlayers()
{
	if (!UGameplayStatics::GetPlayerState(GetWorld(), 0)) return;

	GetWorldTimerManager().PauseTimer(ControllerTimer);

	if (!IsLocalController()) return;

	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	if (!GameState) return;

	TSubclassOf<class UAtlant_W_Menu> MenuWidgetClass;

	IAtlant_I_FromGameState* GameStateI = Cast<IAtlant_I_FromGameState>(GameState);
	if (!GameStateI) return;

	TSubclassOf<class UAtlant_W_Menu> WidgetClass = GameStateI->GetMenuWidgetClass();
	if (!WidgetClass) return;

	MenuWidget = CreateWidget<UAtlant_W_Menu>(this, WidgetClass);
	if (!MenuWidget) return;

	MenuWidget->AddToViewport();
}
