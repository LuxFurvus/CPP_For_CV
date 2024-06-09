// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "E_ItemTypes.generated.h"

UENUM(BlueprintType)
enum class E_ItemTypes : uint8
{
	NONE = 0    UMETA(DisplayName = "NONE"),
	CUBE = 1    UMETA(DisplayName = "Cube"),
	CONE = 2    UMETA(DisplayName = "Cone"),
	BALL = 3    UMETA(DisplayName = "Ball"),
};
