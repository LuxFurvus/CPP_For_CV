// Fill out your copyright notice in the Description page of Project Settings.

#include "C_GameManager.h"
////////////////////////////
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
///////////////////////////////////
#include "C_PlayerBase.h"

void AC_GameManager::BeginPlay()
{
	FTimerHandle DummyHandle;
	GetWorldTimerManager().SetTimer(DummyHandle, [this]() { PayWage(); }, WagePaymentPeriod, true);
}

void AC_GameManager::PayWage()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AC_PlayerBase::StaticClass(), FoundActors);

	for (AActor* SomeActor : FoundActors)
	{
		AC_PlayerBase* PlayerBase{ Cast<AC_PlayerBase>(SomeActor) };
		if (!PlayerBase->GetOwner() || PlayerBase->IsDead) return;

		int32 MoneyToAdd{ static_cast<int32>(GetGameTimeSinceCreation() / WageRisePeriod) * InitialWage };

		PlayerBase->ChangeMoneyOfBase(MoneyToAdd);
	}
	return;
}
