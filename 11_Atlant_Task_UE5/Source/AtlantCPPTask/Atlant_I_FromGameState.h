// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "E_ItemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "Atlant_I_FromGameState.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAtlant_I_FromGameState : public UInterface
{
	GENERATED_BODY()
};

/** * */
class ATLANTCPPTASK_API IAtlant_I_FromGameState
{
	GENERATED_BODY()

public:
	virtual E_ItemTypes GetItemType() const = 0;
	virtual int32 GetItemTotal() const = 0;
	virtual int32 GetItemCollected() const = 0;
	virtual int32 GetTimeLeft() const = 0;
	virtual bool GetIsEnd() const = 0;
	virtual bool GetIsWin() const = 0;
	virtual void AddItemCollected() = 0;
	virtual TSubclassOf<class UAtlant_W_Menu> GetMenuWidgetClass() const = 0;
};
