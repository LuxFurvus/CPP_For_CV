// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Atlant_W_Menu.h"
#include "Atlant_I_ForPlayerController.h"
#include "Atlant_U_InteractComponent.h"
#include "Atlant_A_PlayerController.generated.h"

/** * * * * * * * * * * * * * * * * **/

UCLASS(NotBlueprintable)
class ATLANTCPPTASK_API AAtlant_A_PlayerController final : public APlayerController, public IAtlant_I_ForPlayerController
{
	GENERATED_BODY()	

	FTimerHandle ControllerTimer;
	UAtlant_W_Menu* MenuWidget = nullptr;
	UAtlant_U_InteractComponent* InteractComp = nullptr;

	void BeginPlay() override;
	void WaitForPlayers();
	inline void OnCollectItem() override { MenuWidget->OnCollection(); }
	inline void ShowInteractHint(bool IsInteractable) override { MenuWidget->ShowHideHint(IsInteractable); }
};
