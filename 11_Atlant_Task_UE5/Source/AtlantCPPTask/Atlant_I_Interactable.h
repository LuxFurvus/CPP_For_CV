// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "E_ItemTypes.h"
#include "Atlant_I_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAtlant_I_Interactable : public UInterface
{
	GENERATED_BODY()
};

/** * */

class ATLANTCPPTASK_API IAtlant_I_Interactable
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Caller) = 0;
	virtual bool GetIsInteractable() const = 0;
};
