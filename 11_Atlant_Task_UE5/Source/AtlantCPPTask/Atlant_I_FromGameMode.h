// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E_ItemTypes.h"
#include "Atlant_W_Menu.h"
#include "UObject/Interface.h"
#include "Atlant_I_FromGameMode.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAtlant_I_FromGameMode : public UInterface
{
	GENERATED_BODY()
};

/** * * * * * * * * * * * * * * * * * */
class ATLANTCPPTASK_API IAtlant_I_FromGameMode
{
	GENERATED_BODY()

public:
	virtual E_ItemTypes GetItemType() const = 0;
	virtual TSubclassOf<class UAtlant_W_Menu> GetMenuWidgetClass() const = 0;
	virtual USoundBase* GetNotifySound() const = 0;
	virtual int32 GetTimeAfterEnd() const = 0;
	virtual int32 GetTimeBeforeEnd() const = 0;
	virtual int32 GetItemTotal() const = 0;

};
