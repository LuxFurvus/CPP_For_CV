// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_MenuConnect.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_MenuConnect : public UInterface { GENERATED_BODY() };

class DGMATASK_API II_MenuConnect {
	GENERATED_BODY()

public:	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScreenMaker")
	void MakeEndScreenBP();

public:	UFUNCTION() 
	virtual void MakeEndScreen() = 0;
};
