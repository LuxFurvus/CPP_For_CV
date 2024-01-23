// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_BuildGround.generated.h"

UCLASS()
class DGMATASK_API AC_BuildGround : public AActor
{
	GENERATED_BODY()
	
public:	AC_BuildGround();
protected: virtual void BeginPlay() override;
public:	virtual void Tick(float DeltaTime) override;
};
