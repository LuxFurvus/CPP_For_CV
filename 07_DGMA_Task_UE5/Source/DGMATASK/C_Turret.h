// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "F_TurretTraits.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
////////////////////////////////////
#include "C_Turret.generated.h"

UCLASS()
class DGMATASK_API AC_Turret : public APawn
{
	GENERATED_BODY()

//protected: UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing= OnHealthChanged, Category = TO_SET)
//		 float Health{0.0f};
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TO_SET)
	AActor* Target{nullptr};
protected: UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TO_SET)
	float Radius{5000.0f};
protected: UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TO_SET)
	float ReactionDelay{0.5f};
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = TO_SET)
	bool IsDead;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TO_SET) 
	UMaterialInstanceDynamic* MID_Turret{ nullptr };
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TO_SET) 
	TArray<AActor*> Targets;
protected: UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = TO_SET, meta = (ExposeOnSpawn = true))
	F_TurretTraits TurretTraits;
protected: UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* TurretBase{ nullptr };
protected: UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* TurretGun{ nullptr };
protected: UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* FirePoint{ nullptr };

		 //void OnHealthChanged();


public:	AC_Turret();
public:	virtual void Tick(float DeltaTime) override;
protected:	virtual void BeginPlay() override;
public: void SetTurretTraits(const F_TurretTraits& Traits);


	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
