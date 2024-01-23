// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Containers/Array.h"
#include "Particles/ParticleSystem.h"
#include "I_MarkBaseConnect.h"
////////////////////////////////////
#include "C_PlayerBase.generated.h"

UCLASS(Blueprintable)
class DGMATASK_API AC_PlayerBase : public AActor, public II_MarkBaseConnect
{
	GENERATED_BODY()
	
public: bool IsDead{ false };

protected: UPROPERTY(ReplicatedUsing = OnHealthChanged)
	float Health{ 100.0f };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	float HealthMax{ 100.0f };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	float HealthRegenScale{ 1.0f };

protected: UPROPERTY(ReplicatedUsing = OnMoneyChanged)
	int32 Money{ 0 };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	int32 MoneyMaxForBase{ 100 };

protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	UParticleSystem* BaseEndParticle{ nullptr };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* Platform{ nullptr };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* Spire01{ nullptr };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* Spire02{ nullptr };

private: FTimerHandle RegenTimer;
private: TArray<UMaterialInstanceDynamic*> MID_Mats;

private: const float MinSpireScale{ 15.0f };
private: const float MaxSpireScale{ 45.0f };

protected: UFUNCTION(BlueprintCallable, Server, Reliable)
	void ForHealthChange_SERVER();

public: FORCEINLINE int32 GetBaseMoney() override { return Money; }
private: FORCEINLINE bool CheckForBase() override { return true; };
private: void ClaimBase() override;
public: void ChangeHealthOfBase(float AddHealth) override;

public: UFUNCTION(BlueprintCallable)
	void ChangeMoneyOfBase(int32 AddMoney) override;
private: UFUNCTION() void OnHealthChanged();
private: UFUNCTION() void OnMoneyChanged();
private: void RegenBaseHealth();

public:	AC_PlayerBase();
public:	void Tick(float DeltaTime) override;
protected: void BeginPlay() override;

///////////////////FOR_BP_TESTING_ONLY
public:	bool CheckForBaseBP_Implementation() override;
public:	void ClaimBaseBP_Implementation() override;
public:	void ChangeHealthOfBaseBP_Implementation(float AddHealth) override;
public:	void ChangeMoneyOfBaseBP_Implementation(int32 AddMoney) override;
};
