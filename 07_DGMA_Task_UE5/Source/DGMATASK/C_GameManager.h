// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/GameModeBase.h"
///////////////////////////////////
#include "C_GameManager.generated.h"

/** *  */
UCLASS()
class DGMATASK_API AC_GameManager : public AGameModeBase
{
	GENERATED_BODY()
	
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TO_SET)
	int32 InitialWage{ 1 };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TO_SET)
	float WageRisePeriod{ 10.0f };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TO_SET)
	float WagePaymentPeriod{ 2.5f };

protected: void BeginPlay() override;

protected: void PayWage();

};
