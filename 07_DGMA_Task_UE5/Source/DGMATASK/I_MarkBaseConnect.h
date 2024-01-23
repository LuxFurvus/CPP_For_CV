// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_MarkBaseConnect.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_MarkBaseConnect : public UInterface { GENERATED_BODY() };

class DGMATASK_API II_MarkBaseConnect {
	GENERATED_BODY()

public:	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSelect")
	bool CheckForBaseBP();
public:	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BaseSelect")
	void ClaimBaseBP();
public:	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChangeStats")
	void ChangeHealthOfBaseBP(float AddHealth);
public:	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ChangeStats")
	void ChangeMoneyOfBaseBP(int32 AddMoney);

public:	UFUNCTION()
	virtual bool CheckForBase() = 0;
public:	UFUNCTION() 
	virtual void ClaimBase() = 0;
public:	UFUNCTION() 
	virtual int32 GetBaseMoney() = 0;
public:	UFUNCTION() 
	virtual void ChangeHealthOfBase(float AddHealth) = 0;
public:	UFUNCTION() 
	virtual void ChangeMoneyOfBase(int32 AddMoney) = 0;
};
