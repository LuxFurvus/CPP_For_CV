// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "E_ItemTypes.h"
#include "Atlant_I_FromGameState.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Atlant_A_GameState.generated.h"

UCLASS(NotBlueprintable)
class ATLANTCPPTASK_API AAtlant_A_GameState final : public AGameStateBase, public IAtlant_I_FromGameState
{
	GENERATED_BODY()

	enum class E_SessionState
	{
		MAIN_PART = 0    UMETA(DisplayName = "Main Game Period"),
		VICTORY = 1    UMETA(DisplayName = "After Victory Period"),
		DEFEAT = 2    UMETA(DisplayName = "After Defeat Period")
	};

private:
	UPROPERTY(Replicated)
	TSubclassOf<class UAtlant_W_Menu> MenuWidgetClass = nullptr;
	UPROPERTY(Replicated)
	USoundBase* CollectNotifySound = nullptr;
	UPROPERTY(Replicated)
	E_ItemTypes CurrentItemType = E_ItemTypes::NONE;
	UPROPERTY(Replicated)
	int32 TimeBeforeEnd = 15.0f;
	UPROPERTY(Replicated)
	int32 TimeAfterEnd = 15.0f;
	UPROPERTY(Replicated)
	int32 ItemTotal = 1;
	UPROPERTY(ReplicatedUsing = OnRep_ItemCollected)
	int32 ItemCollected = 0;

	FTimerHandle StateTimer;
	E_SessionState SessionStatus = E_SessionState::MAIN_PART;
	int32 GameEndMoment = 0.0f;
	int32 TimeLeft = 0.0f;
	bool IsEnd = false;
	bool IsWin = false;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ItemCollected();

	UFUNCTION(NetMulticast, Unreliable)
	void MULTI_QuitAllPlayers();
	void MULTI_QuitAllPlayers_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void MULTI_DoAscension();
	void MULTI_DoAscension_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void MULTI_NotifyPlayers();
	void MULTI_NotifyPlayers_Implementation();

public:
	AAtlant_A_GameState();
	void BeginPlay() override;
	void DoCountdown();

	inline E_ItemTypes GetItemType() const override { return CurrentItemType; }
	inline int32 GetItemTotal() const override { return ItemTotal; }
	inline int32 GetItemCollected() const override { return ItemCollected; }
	inline int32 GetTimeLeft() const override { return TimeLeft; }
	inline bool GetIsEnd() const override { return IsEnd; }
	inline bool GetIsWin() const override { return IsWin; }
	inline void AddItemCollected() override { ++ItemCollected; }
	inline TSubclassOf<class UAtlant_W_Menu> GetMenuWidgetClass() const override { return MenuWidgetClass; }
};
