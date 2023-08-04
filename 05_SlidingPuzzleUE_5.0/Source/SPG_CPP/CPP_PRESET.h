//Copyright (c) 2023 Swen Willson (MagnumFalls). All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "UObject/NoExportTypes.h"
#include "CPP_PRESET.generated.h"

UENUM()
enum class E_StartCorner : uint8 {
	EO_ALL = 0			UMETA(DisplayName = "ALL"),
	EO_LeftDown = 1		UMETA(DisplayName = "LeftDown"),
	EO_RightDown = 2	UMETA(DisplayName = "RightDown"),
	EO_LeftUp = 3		UMETA(DisplayName = "LeftUp"),
	EO_RightUp = 4		UMETA(DisplayName = "RightUp"),
	EO_RANDOM = 5		UMETA(DisplayName = "RANDOM")
};

UCLASS(Blueprintable, BlueprintType, Abstract)
class SPG_CPP_API UCPP_PRESET : public UObject {
	GENERATED_BODY()

public:
	//////////// GENERAL SETTINGS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General")
		bool ShowNumbers = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General", meta = (EditCondition = "ShowNumbers == true", EditConditionHides))
		bool DoubleNumbers = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 2, ClampMax = 10), Category = "1_General")
		int32 PlateNum = 4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General")
		E_StartCorner CornerToStart = E_StartCorner::EO_RANDOM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "1_General")
		UTexture* ImageToPlay = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "1_General")
		UFont* Font_RenderText = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "1_General")
		UFont* Font_Widget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General")
		FLinearColor GoodGroundColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General")
		FLinearColor FreeGroundColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General")
		FLinearColor WrongGroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "1_General")
		FLinearColor RenderTextColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//////////// MISCELLANEOUS SETTINGS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 5), Category = "2_Misc")
		float BorderGapSize = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		UStaticMesh* SM_Plate = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		UMaterialInstance* Mi_Plate = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		UMaterialInstance* Mi_Ground = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		UMaterialInstance* Mi_Text = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		USoundCue* SC_Win = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		USoundCue* SC_Move = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "2_Misc")
		TSubclassOf<UUserWidget> PuzzleWidget = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 25), Category = "2_Misc")
		float GoodGroundEmi = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 25), Category = "2_Misc")
		float FreeGroundEmi = 0.1f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 25), Category = "2_Misc")
		float WrongGroundEmi = 0.0f;
};
