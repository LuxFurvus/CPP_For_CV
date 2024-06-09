// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Atlant_I_FromGameState.h"
#include "Atlant_W_Menu.generated.h"

/** *  */
UCLASS(Blueprintable)
class ATLANTCPPTASK_API UAtlant_W_Menu final : public UUserWidget
{
	GENERATED_BODY()

	FTimerHandle HUDTimer;
	IAtlant_I_FromGameState* GameStateI = nullptr;
	bool IsEnd = false;
	void NativeConstruct() override;
	void WidgetUpdater();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TX_ItemType = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TX_ItemCollected = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TX_ItemTotal = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TX_FinalResult = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TX_TimeLeft = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TX_Hint = nullptr;

public:
	void OnCollection();
	void ShowHideHint(bool IsInteractable);
};
