// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Atlant_I_ForPlayerController.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAtlant_I_ForPlayerController : public UInterface
{
	GENERATED_BODY()
};

/** *  */
class ATLANTCPPTASK_API IAtlant_I_ForPlayerController
{
	GENERATED_BODY()
public:
    virtual void OnCollectItem() = 0;
	virtual void ShowInteractHint(bool IsInteractable) = 0;
};
