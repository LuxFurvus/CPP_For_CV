// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "E_ItemTypes.h"
#include "Atlant_A_GameState.h"
#include "Atlant_I_FromGameMode.h"
#include "Atlant_A_GameMode.generated.h"

UCLASS(Blueprintable)
class AAtlant_A_GameMode final : public AGameModeBase, public IAtlant_I_FromGameMode
{
	GENERATED_BODY()

private:
	E_ItemTypes MainItemType{ E_ItemTypes::NONE };
public:
	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	TSubclassOf<class UAtlant_W_Menu> MenuWidgetClass = nullptr;

	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category = Classes)
	USoundBase* CollectNotifySound;

	UPROPERTY(EditDefaultsOnly, NoClear, BlueprintReadOnly, Category = Classes)
	int32 TimeBeforeEnd = 15.0f;

	UPROPERTY(EditDefaultsOnly, NoClear, BlueprintReadOnly, Category = Classes)
	int32 TimeAfterEnd = 15.0f;

	UPROPERTY(EditDefaultsOnly, NoClear, BlueprintReadOnly, Category = Classes)
	int32 ItemTotal = 1;

	AAtlant_A_GameMode();
	void BeginPlay() override final;
	inline E_ItemTypes GetItemType() const override {return MainItemType; }
	inline TSubclassOf<class UAtlant_W_Menu> GetMenuWidgetClass() const override { return MenuWidgetClass; }
	inline USoundBase* GetNotifySound() const override { return CollectNotifySound; }
	inline int32 GetTimeBeforeEnd() const override { return TimeBeforeEnd; }
	inline int32 GetTimeAfterEnd() const override { return TimeAfterEnd; }
	inline int32 GetItemTotal() const override { return ItemTotal; }
};



