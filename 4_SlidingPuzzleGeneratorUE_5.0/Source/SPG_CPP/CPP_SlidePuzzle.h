//Copyright (c) 2023 Swen Willson (MagnumFalls). All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundCue.h"
#include "Engine/Font.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

#include "I_MENUConnect.h"
#include "CPP_PRESET.h"
#include "CPP_SlidePuzzle.generated.h"

UCLASS()
class SPG_CPP_API ACPP_SlidePuzzle : public APawn, public II_MENUConnect {
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	enum E_AxisPrecedence {
		EO_XisY,
		EO_UP,
		EO_DOWN,
		EO_LEFT,
		EO_RIGHT
	};

	struct FGroundStats {
		UStaticMeshComponent* OriginalPlate;
		UStaticMeshComponent* CurrentPlate;

		FGroundStats() {
			OriginalPlate = nullptr;
			CurrentPlate = nullptr;
		}
	};

	/////1_General
	bool ShowNumbers = true;
	bool DoubleNumbers = false;
	uint8 PlateNum = 2;
	float BorderGapSize = 1.f;
	E_StartCorner CornerToStart = E_StartCorner::EO_RANDOM;

	/////2_Refs
	UStaticMesh* SM_Plate = nullptr;
	UTexture* ImageToPlay = nullptr;
	UMaterialInstance* Mi_Plate = nullptr;
	UMaterialInstanceDynamic* MID_Good = nullptr;
	UMaterialInstanceDynamic* MID_Wrong = nullptr;
	UMaterialInstanceDynamic* MID_Free = nullptr;
	UMaterialInstanceDynamic* MID_RenderText = nullptr;
	UFont* Font_RenderText = nullptr;
	USoundCue* SC_Win = nullptr;
	USoundCue* SC_Move = nullptr;
	TSubclassOf<UUserWidget> PuzzleWidgetClass = nullptr;
	UUserWidget* PuzzleWidget = nullptr;

	/////ConstructGlobals
	float UniformScaleModifier;
	float AdjustedPlateScale;
	uint8 MaxXY;
	uint8 TotalPlateCount = 0;
	float PlateXYScale;
	float PlateJamScale;
	float BorderOffset;
	FVector GroundScale;

	const FName PlateIdentifier = FName(TEXT("PLATE"));

	TArray<UStaticMeshComponent*> GroundARRAY;
	TArray<UStaticMeshComponent*> PlateARRAY;
	TArray<UStaticMeshComponent*> FreeGroundARRAY;
	TArray<UTextRenderComponent*> NumbersARRAY;
	TMap<UStaticMeshComponent*, FGroundStats> GroundStatsMAP;

	///////RuntimeGlobals
	bool HasGameEnded = false;
	bool IsPlateMoving = false;
	bool GrabMode = false;
	uint8 GoodGroundNum = 0;
	TSet<E_AxisPrecedence> SidesToMoveSET;
	TSet<UStaticMeshComponent*> SelectablePlatesSET;
	APlayerController* PlayerController = nullptr;
	FTimerHandle TH_HideBorders;
	FTimerHandle TH_SlidePlate;
	UStaticMeshComponent* SelectedPlate;
	UStaticMeshComponent* MovingPlate;
	UInputComponent* PlayerInputComp;
	E_AxisPrecedence AxisPrecedence = EO_XisY;
	FVector PlateTarget;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////
	////////////////////////////////////////PRIVATE FUNCTIONS////////////////////////////////////////
	////////////////////////////////////////PRIVATE FUNCTIONS/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////CONSTRUCTION//////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	virtual void OnConstruction(const FTransform& Transform) override;
	void CS_SetCameraComponents();
	void CS_OpenPreset();
	void CS_SetConstructGlobals();
	void CS_AddPlatesAndGrounds();
	void CS_DecoratePlate(
		uint8 CountXloc,
		uint8 CountYloc,
		UStaticMeshComponent* NewGround,
		UStaticMeshComponent* NewPlate);
	FTransform CS_SetTextTransform(FTransform GroundTransform);
	void CS_AddTextNumbers(
		UStaticMeshComponent* ParentPlate,
		FTransform GroundTransform);
	void CS_MakeFreeGround(float XValue, float YValue);
	void CS_SelectFreeGround();
	template<typename T>
	void CS_ShuffleArray(const TArray<T>& ArrayToSuffle);
	void CS_SetPlateTransform(UStaticMeshComponent* NewGround, UStaticMeshComponent* NewPlate);
	void CS_ArrangePlates();
	void CheckAllGrounds();

	/////////////////////////////////////INPUT//////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	void LMBPressed();
	void LMBReleased();
	void AxisStealer(float axis);

	/////////////////////////////////////ONE TIME FUNCTIONS//////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	void StartGame();
	void SetArmLength();
	void HideBorders();

	/////////////////////////////////////BEFORE EVERY MOVE//////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	void MarkActivePlates();
	UStaticMeshComponent* RealPlate(FHitResult ActiveHit);
	void ScanPlate();
	void Scan_SelectPlate();
	UStaticMeshComponent* GetGroundBelow();
	FVector GetUnderPlateVector(UStaticMeshComponent* PlateAbove);
	/////////////////////////////////////DURING EVERY MOVE//////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	void Scan_AxisControl();
	void Scan_GetAllPlateDirections();
	bool CheckFGDistance(UStaticMeshComponent* FGtoCheck) const;
	void StartPlateMovement();
	void SlidePlate();
	void IfGoodGround(UStaticMeshComponent* Ground, bool IsAfterMove);
	void CheckOneGround(UStaticMeshComponent* GroundToCheck);
	void AfterEveryMove() const;

	/*...................................................................................*/
	/*...................................................................................*/
	/*...................................................................................*/

	/*...................................................................................*/
	/*...................................................................................*/
	/*...................................................................................*/

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////PUBLIC//////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

public:
	ACPP_SlidePuzzle();

	UPROPERTY(VisibleDefaultsOnly, Category = "COMPONENTS")
		USceneComponent* BaseRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "PRESET")
		USpringArmComponent* SpringArmComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "PRESET")
		UCameraComponent* CameraComp;
	UPROPERTY(VisibleDefaultsOnly, Category = "COMPONENTS")
		UStaticMeshComponent* EditorPlane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, Category = "PRESET")
		TSubclassOf<UCPP_PRESET> PRESET;
	UPROPERTY(EditInstanceOnly, Category = "PRESET")
		bool ShufflePlates = true;
};
