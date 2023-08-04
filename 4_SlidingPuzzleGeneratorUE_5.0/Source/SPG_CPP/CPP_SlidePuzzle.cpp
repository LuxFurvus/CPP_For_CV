//Copyright (c) 2023 Swen Willson (MagnumFalls). All rights reserved.

#include "CPP_SlidePuzzle.h"

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////CONSTRUCTION//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

ACPP_SlidePuzzle::ACPP_SlidePuzzle() {
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = nullptr;
	CS_SetCameraComponents();
}
void ACPP_SlidePuzzle::CS_SetCameraComponents() {
	BaseRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisibleRoot"));
	BaseRoot->SetupAttachment(RootComponent);
	BaseRoot->SetComponentTickEnabled(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CPP_SpringArm"));
	SpringArmComp->SetupAttachment(BaseRoot);
	SpringArmComp->SetRelativeLocation(FVector(200.0f, 200.0f, 0.f));
	SpringArmComp->TargetArmLength = 500.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->SetCameraMesh(nullptr);
	CameraComp->SetComponentTickEnabled(false);

	EditorPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EditorPlane"));
	EditorPlane->SetupAttachment(BaseRoot);
	EditorPlane->SetRelativeLocation(FVector(200.0f, 200.0f, 0.0f));
	EditorPlane->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
	EditorPlane->SetHiddenInGame(true);
	EditorPlane->SetCollisionProfileName(FName("NoCollision"));
	EditorPlane->SetGenerateOverlapEvents(false);
	EditorPlane->SetComponentTickEnabled(false);
}
void ACPP_SlidePuzzle::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	float UniScale = FMath::Clamp(GetActorScale3D().X, 0.017f, GetActorScale3D().X);

	SetActorScale3D(FVector(
		UniScale,
		UniScale,
		UniScale)
	);
}
void ACPP_SlidePuzzle::CS_OpenPreset() {
	///////1_General
	ShowNumbers = PRESET->GetDefaultObject<UCPP_PRESET>()->ShowNumbers;
	DoubleNumbers = PRESET->GetDefaultObject<UCPP_PRESET>()->DoubleNumbers;
	PlateNum = PRESET->GetDefaultObject<UCPP_PRESET>()->PlateNum;
	BorderGapSize = PRESET->GetDefaultObject<UCPP_PRESET>()->BorderGapSize;
	CornerToStart = PRESET->GetDefaultObject<UCPP_PRESET>()->CornerToStart;
	///////2_Refs
	SM_Plate = PRESET->GetDefaultObject<UCPP_PRESET>()->SM_Plate;
	ImageToPlay = PRESET->GetDefaultObject<UCPP_PRESET>()->ImageToPlay;
	Mi_Plate = PRESET->GetDefaultObject<UCPP_PRESET>()->Mi_Plate;
	Font_RenderText = PRESET->GetDefaultObject<UCPP_PRESET>()->Font_RenderText;
	SC_Win = PRESET->GetDefaultObject<UCPP_PRESET>()->SC_Win;
	SC_Move = PRESET->GetDefaultObject<UCPP_PRESET>()->SC_Move;
	PuzzleWidgetClass = PRESET->GetDefaultObject<UCPP_PRESET>()->PuzzleWidget;
	UMaterialInstance* Mi_Temp = PRESET->GetDefaultObject<UCPP_PRESET>()->Mi_Ground;

	///GOOD GROUND MID
	MID_Good = UMaterialInstanceDynamic::Create(Mi_Temp, this);
	float GoodGroundEmi = PRESET->GetDefaultObject<UCPP_PRESET>()->GoodGroundEmi;
	FLinearColor GoodGroundColor = PRESET->GetDefaultObject<UCPP_PRESET>()->GoodGroundColor;
	MID_Good->SetScalarParameterValue(TEXT("EMI"), GoodGroundEmi);
	MID_Good->SetVectorParameterValue(TEXT("COLOR"), GoodGroundColor);

	///FREE GROUND MID
	MID_Free = UMaterialInstanceDynamic::Create(Mi_Temp, this);
	float FreeGroundEmi = PRESET->GetDefaultObject<UCPP_PRESET>()->FreeGroundEmi;
	FLinearColor FreeGroundColor = PRESET->GetDefaultObject<UCPP_PRESET>()->FreeGroundColor;
	MID_Free->SetScalarParameterValue(TEXT("EMI"), FreeGroundEmi);
	MID_Free->SetVectorParameterValue(TEXT("COLOR"), FreeGroundColor);

	///WRONG GROUND MID
	MID_Wrong = UMaterialInstanceDynamic::Create(Mi_Temp, this);
	float WrongGroundEmi = PRESET->GetDefaultObject<UCPP_PRESET>()->WrongGroundEmi;
	FLinearColor WrongGroundColor = PRESET->GetDefaultObject<UCPP_PRESET>()->WrongGroundColor;
	MID_Wrong->SetScalarParameterValue(TEXT("EMI"), WrongGroundEmi);
	MID_Wrong->SetVectorParameterValue(TEXT("COLOR"), WrongGroundColor);

	///RENDER TEXT MID
	Mi_Temp = PRESET->GetDefaultObject<UCPP_PRESET>()->Mi_Text;
	MID_RenderText = UMaterialInstanceDynamic::Create(Mi_Temp, this);
	FLinearColor RenderTextColor = PRESET->GetDefaultObject<UCPP_PRESET>()->RenderTextColor;
	MID_RenderText->SetVectorParameterValue(TEXT("COLOR"), RenderTextColor);
}
void ACPP_SlidePuzzle::CS_SetConstructGlobals() {
	UniformScaleModifier = GetActorScale3D().X;

	MaxXY = PlateNum - 1;

	float temp = 4.0f / static_cast<float>(PlateNum);
	GroundScale = FVector(temp);

	PlateXYScale = 400.0f / static_cast<float>(PlateNum);
	AdjustedPlateScale = PlateXYScale * UniformScaleModifier;

	PlateJamScale = 1.0f - (BorderGapSize * 0.1f);
	BorderOffset = 0.5f * PlateXYScale * (1 - PlateJamScale);
}
void ACPP_SlidePuzzle::CS_AddPlatesAndGrounds() {
	float LocationY;
	uint8 CountX = -1;
	uint8 CountY = -1;
	TotalPlateCount = 0;
	FTransform GroundTransform;

	for (uint8 iOuter = 0; iOuter <= MaxXY; iOuter++) {
		CountX++;
		LocationY = iOuter * PlateXYScale;

		for (uint8 iInner = 0; iInner <= MaxXY; iInner++) {
			CountY++;
			TotalPlateCount++;
			CountY = CountY % PlateNum;

			float tempX = PlateXYScale * iInner;
			float tempY = LocationY;
			float tempZ = 0.f;
			GroundTransform.SetLocation(FVector(tempX, tempY, 0.f));
			GroundTransform.SetRotation(FQuat(0.f, 0.f, 0.f, 0.f));
			GroundTransform.SetScale3D(GroundScale);

			UActorComponent* CompToCast = AddComponentByClass(
				UStaticMeshComponent::StaticClass(),
				false,
				GroundTransform,
				false
			);
			UStaticMeshComponent* NewGround = Cast<UStaticMeshComponent>(CompToCast);
			GroundARRAY.Add(NewGround);
			NewGround->SetComponentTickEnabled(false);

			CompToCast = AddComponentByClass(
				UStaticMeshComponent::StaticClass(),
				false,
				GroundTransform,
				false
			);
			UStaticMeshComponent* NewPlate = Cast<UStaticMeshComponent>(CompToCast);
			PlateARRAY.Add(NewPlate);
			NewPlate->SetComponentTickEnabled(false);

			CS_DecoratePlate(CountX, CountY, NewGround, NewPlate);
			if (ShowNumbers == true) {
				CS_AddTextNumbers(NewPlate, GroundTransform);
			}
		}
	}
}
void ACPP_SlidePuzzle::CS_DecoratePlate(
	uint8 CountXloc,
	uint8 CountYloc,
	UStaticMeshComponent* NewGround,
	UStaticMeshComponent* NewPlate) {
	NewGround->SetStaticMesh(SM_Plate);
	NewPlate->SetStaticMesh(SM_Plate);

	FGroundStats StrToAdd;
	StrToAdd.OriginalPlate = NewPlate;
	StrToAdd.CurrentPlate = nullptr;
	GroundStatsMAP.Add(NewGround, StrToAdd);

	NewPlate->ComponentTags.Add(PlateIdentifier);
	FName num = (*FString::FromInt(CountXloc));
	NewPlate->ComponentTags.Add(num);
	num = (*FString::FromInt(CountYloc));
	NewPlate->ComponentTags.Add(num);
	num = (*FString::FromInt(TotalPlateCount));
	NewPlate->ComponentTags.Add(num);

	UMaterialInstanceDynamic* MID_ForOnePlate = UMaterialInstanceDynamic::Create(Mi_Plate, this);
	NewPlate->SetMaterial(0, MID_ForOnePlate);
	MID_ForOnePlate->SetScalarParameterValue(TEXT("XPos"), CountXloc);
	MID_ForOnePlate->SetScalarParameterValue(TEXT("YPos"), CountYloc);
	float TileSize = 10 / (static_cast<float>(PlateNum));
	MID_ForOnePlate->SetScalarParameterValue(TEXT("TileSize"), TileSize);
	MID_ForOnePlate->SetTextureParameterValue(TEXT("Image"), ImageToPlay);
}
FTransform ACPP_SlidePuzzle::CS_SetTextTransform(FTransform GroundTransform) {
	float TextXYPos = (2.0f / static_cast<float>(PlateNum)) * 100.f;

	float tempX = GroundTransform.GetLocation().X + TextXYPos;
	float tempY = GroundTransform.GetLocation().Y + TextXYPos;
	float tempZ = GroundTransform.GetLocation().Z + 1;
	GroundTransform.SetLocation(FVector(tempX, tempY, tempZ));

	tempX = GroundTransform.GetRotation().X + 90;
	tempY = GroundTransform.GetRotation().Y - 90;
	tempZ = GroundTransform.GetRotation().Z + 90;
	FRotator tempRot = FRotator(tempX, tempY, tempZ);
	GroundTransform.SetRotation(FQuat(tempRot));

	tempX = GroundTransform.GetScale3D().X * 2.25f;
	tempY = GroundTransform.GetScale3D().Y * 2.25f;
	tempZ = GroundTransform.GetScale3D().Z * 2.25f;
	GroundTransform.SetScale3D(FVector(tempX, tempY, tempZ));
	return GroundTransform;
}
void ACPP_SlidePuzzle::CS_AddTextNumbers(
	UStaticMeshComponent* ParentPlate,
	FTransform GroundTransform) {
	FTransform NewGroundTransform = CS_SetTextTransform(GroundTransform);
	UActorComponent* CompToCast = AddComponentByClass(
		UTextRenderComponent::StaticClass(),
		false,
		NewGroundTransform,
		false
	);
	UTextRenderComponent* TextNum = Cast<UTextRenderComponent>(CompToCast);
	NumbersARRAY.Add(TextNum);
	TextNum->SetComponentTickEnabled(false);

	FAttachmentTransformRules tempRules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		false);
	TextNum->AttachToComponent(ParentPlate, tempRules);

	TextNum->SetMaterial(0, MID_RenderText);
	TextNum->SetFont(Font_RenderText);
	FText NumToSet;
	if (DoubleNumbers == true) {
		FString Duo = ParentPlate->ComponentTags[1].ToString();
		FString ToDou = ParentPlate->ComponentTags[2].ToString();
		Duo += ToDou;
		NumToSet = FText::FromString(Duo);
	}
	else {
		FName Single = ParentPlate->ComponentTags[3];
		NumToSet = FText::FromName(Single);
	}
	TextNum->SetText(NumToSet);
	TextNum->SetVerticalAlignment(EVRTA_TextCenter);
	TextNum->SetHorizontalAlignment(EHTA_Center);
	TextNum->SetVisibility(true);
}
void ACPP_SlidePuzzle::CS_MakeFreeGround(float XValue, float YValue) {
	FVector loc = FVector(XValue, YValue, 0.f);
	FQuat rot = FQuat(FRotator(0.f, 0.f, 0.f));
	FTransform FGTransform = FTransform(rot, loc, GroundScale);
	UActorComponent* comtToCast = AddComponentByClass(
		UStaticMeshComponent::StaticClass(),
		false,
		FGTransform,
		false);
	UStaticMeshComponent* NewFG = Cast<UStaticMeshComponent>(comtToCast);
	NewFG->SetComponentTickEnabled(false);

	FGroundStats StrToAdd;
	StrToAdd.OriginalPlate = nullptr;
	StrToAdd.CurrentPlate = nullptr;
	GroundStatsMAP.Add(NewFG, StrToAdd);

	FreeGroundARRAY.Add(NewFG);
	NewFG->SetStaticMesh(SM_Plate);
}
void ACPP_SlidePuzzle::CS_SelectFreeGround() {
	float SuperMinXY = -PlateXYScale;
	float UsualMaxXY = PlateXYScale * static_cast<float>(MaxXY);
	float SuperMaxXY = UsualMaxXY + PlateXYScale;

	enum E_EveryFreeGround : uint8 {
		LeftDown_Left = 0,
		LeftDown_Down = 1,
		RightDown_Down = 2,
		RightDown_Right = 3,
		LeftUp_Left = 4,
		LeftUp_Up = 5,
		RightUp_Up = 6,
		RightUp_Right = 7
	};
	E_EveryFreeGround FreeGround01 = LeftDown_Left;
	E_EveryFreeGround FreeGround02 = LeftDown_Left;
	bool NeedsAllFG = false;

	switch (CornerToStart) {
	case E_StartCorner::EO_LeftDown:
		FreeGround01 = LeftDown_Down;
		FreeGround02 = LeftDown_Left;
		break;
	case E_StartCorner::EO_RightDown:
		FreeGround01 = RightDown_Down;
		FreeGround02 = RightDown_Right;
		break;
	case E_StartCorner::EO_LeftUp:
		FreeGround01 = LeftUp_Up;
		FreeGround02 = LeftUp_Left;
		break;
	case E_StartCorner::EO_RightUp:
		FreeGround01 = RightUp_Up;
		FreeGround02 = RightUp_Right;
		break;
	case E_StartCorner::EO_RANDOM:
		FreeGround01 = static_cast<E_EveryFreeGround>(FMath::RandRange(0, 3));
		FreeGround02 = static_cast<E_EveryFreeGround>(FMath::RandRange(4, 7));
		break;
	case E_StartCorner::EO_ALL:
		NeedsAllFG = true;
		break;
	}
	uint8 LoopNumber = (NeedsAllFG == true) ? 7 : 0;
	for (uint8 i = 0; i <= LoopNumber; i++) {
		FreeGround01 = (NeedsAllFG == true) ? static_cast<E_EveryFreeGround>(i) : FreeGround01;
		float XValue1 = 0.f;
		float YValue1 = 0.f;
		switch (FreeGround01) {
		case E_EveryFreeGround::LeftDown_Left:
			XValue1 = 0.f;
			YValue1 = SuperMinXY;
			break;
		case E_EveryFreeGround::LeftDown_Down:
			XValue1 = SuperMinXY;
			YValue1 = 0.f;
			break;
		case E_EveryFreeGround::RightDown_Down:
			XValue1 = SuperMinXY;
			YValue1 = UsualMaxXY;
			break;
		case E_EveryFreeGround::RightDown_Right:
			XValue1 = 0.f;
			YValue1 = SuperMaxXY;
			break;
		case E_EveryFreeGround::LeftUp_Left:
			XValue1 = UsualMaxXY;
			YValue1 = SuperMinXY;
			break;
		case E_EveryFreeGround::LeftUp_Up:
			XValue1 = SuperMaxXY;
			YValue1 = 0.f;
			break;
		case E_EveryFreeGround::RightUp_Up:
			XValue1 = SuperMaxXY;
			YValue1 = UsualMaxXY;
			break;
		case E_EveryFreeGround::RightUp_Right:
			XValue1 = UsualMaxXY;
			YValue1 = SuperMaxXY;
			break;
		}
		CS_MakeFreeGround(XValue1, YValue1);
	}

	if (NeedsAllFG == true) {
		return;
	}

	float XValue2 = 0.f;
	float YValue2 = 0.f;
	switch (FreeGround02) {
	case E_EveryFreeGround::LeftDown_Left:
		XValue2 = 0.f;
		YValue2 = SuperMinXY;
		break;
	case E_EveryFreeGround::LeftDown_Down:
		XValue2 = SuperMinXY;
		YValue2 = 0.f;
		break;
	case E_EveryFreeGround::RightDown_Down:
		XValue2 = SuperMinXY;
		YValue2 = UsualMaxXY;
		break;
	case E_EveryFreeGround::RightDown_Right:
		XValue2 = 0.f;
		YValue2 = SuperMaxXY;
		break;
	case E_EveryFreeGround::LeftUp_Left:
		XValue2 = UsualMaxXY;
		YValue2 = SuperMinXY;
		break;
	case E_EveryFreeGround::LeftUp_Up:
		XValue2 = SuperMaxXY;
		YValue2 = 0.f;
		break;
	case E_EveryFreeGround::RightUp_Up:
		XValue2 = SuperMaxXY;
		YValue2 = UsualMaxXY;
		break;
	case E_EveryFreeGround::RightUp_Right:
		XValue2 = UsualMaxXY;
		YValue2 = SuperMaxXY;
		break;
	}
	CS_MakeFreeGround(XValue2, YValue2);
}
template<typename T>
void ACPP_SlidePuzzle::CS_ShuffleArray(const TArray<T>& ArrayToSuffle) {
	TArray<T> UnshuffledArray = ArrayToSuffle;

ShuffleAgain:
	int32 LastIndex = ArrayToSuffle.Num() - 1;
	for (int32 i = 0; i < LastIndex; ++i) {
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index) {
			const_cast<TArray<T>*>(&ArrayToSuffle)->Swap(i, Index);
		}
	}

	if (UnshuffledArray == ArrayToSuffle) {
		goto ShuffleAgain;
	}
}
void ACPP_SlidePuzzle::CS_SetPlateTransform(UStaticMeshComponent* NewGround, UStaticMeshComponent* NewPlate) {
	NewPlate->SetRelativeRotation(NewGround->GetRelativeRotation());

	float ScaleX = NewGround->GetRelativeScale3D().X * PlateJamScale;
	float ScaleY = NewGround->GetRelativeScale3D().Y * PlateJamScale;
	float ScaleZ = NewGround->GetRelativeScale3D().Z * PlateJamScale;
	FVector NewScale = FVector(ScaleX, ScaleY, ScaleZ);
	NewPlate->SetRelativeScale3D(NewScale);

	float LocX = NewGround->GetRelativeLocation().X + BorderOffset;
	float LocY = NewGround->GetRelativeLocation().Y + BorderOffset;
	float LocZ = NewGround->GetRelativeLocation().Z + 1.f;
	FVector NewLoc = FVector(LocX, LocY, LocZ);
	NewPlate->SetRelativeLocation(NewLoc);
}
void ACPP_SlidePuzzle::CS_ArrangePlates() {
	if (ShufflePlates == true) {
		CS_ShuffleArray(PlateARRAY);
	}
	for (int32 indi = 0; indi < GroundARRAY.Num(); indi++) {
		UStaticMeshComponent* NewGround = GroundARRAY[indi];
		UStaticMeshComponent* NewPlate = PlateARRAY[indi];

		CS_SetPlateTransform(NewGround, NewPlate);

		GroundStatsMAP.Find(NewGround)->CurrentPlate = NewPlate;
	}
}
void ACPP_SlidePuzzle::CheckAllGrounds() {
	TArray<UStaticMeshComponent*> Grounds;
	GroundStatsMAP.GenerateKeyArray(Grounds);

	for (UStaticMeshComponent* GroundToCheck : Grounds) {
		if (GroundToCheck == nullptr) { continue; }

		FGroundStats* Plates = GroundStatsMAP.Find(GroundToCheck);

		bool IsThereOriginalPlate = (Plates->OriginalPlate != nullptr);
		bool ArePlatesTheSame = (Plates->CurrentPlate == Plates->OriginalPlate);

		bool IsGroundGood = IsThereOriginalPlate && ArePlatesTheSame;

		GoodGroundNum = (IsGroundGood == true) ? GoodGroundNum + 1 : GoodGroundNum;

		CheckOneGround(GroundToCheck);
	}

	AfterEveryMove();
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////INPUT//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void ACPP_SlidePuzzle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComp = PlayerInputComponent;
	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &ACPP_SlidePuzzle::AxisStealer);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &ACPP_SlidePuzzle::AxisStealer);

	PlayerInputComponent->BindKey(
		EKeys::LeftMouseButton,
		IE_Pressed,
		this,
		&ACPP_SlidePuzzle::LMBPressed
	);

	PlayerInputComponent->BindKey(
		EKeys::LeftMouseButton,
		IE_Released,
		this,
		&ACPP_SlidePuzzle::LMBReleased
	);
}
void ACPP_SlidePuzzle::LMBPressed() {
	GrabMode = true;
	if (PlayerController != nullptr) {
		PlayerController->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
	}
}
void ACPP_SlidePuzzle::LMBReleased() {
	GrabMode = false;
	SidesToMoveSET.Empty(4);
	if (PlayerController != nullptr) {
		PlayerController->CurrentMouseCursor = EMouseCursor::Default;
	}
}
void ACPP_SlidePuzzle::AxisStealer(float axis) {
	//Provides data for AxisPrecedence
	//We can use this function as a counterpart of disabled Tick function during debugging
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////ONE TIME FUNCTIONS//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void ACPP_SlidePuzzle::BeginPlay() {
	Super::BeginPlay();

	bool ShallWeBegin =
		PRESET != nullptr
		&& IsPlayerControlled();
	if (ShallWeBegin == false) { return; }

	CS_OpenPreset();
	CS_SetConstructGlobals();
	CS_AddPlatesAndGrounds();
	CS_SelectFreeGround();
	CS_ArrangePlates();
	StartGame();
	MarkActivePlates();
	CheckAllGrounds();
}
void ACPP_SlidePuzzle::StartGame() {
	if (IsPlayerControlled() == false) {
		return;
	}
	PlayerController = GetPlayerState()->GetPlayerController();

	PuzzleWidget = CreateWidget(PlayerController, PuzzleWidgetClass);
	bool IsWidgetCorrect =
		PuzzleWidget != nullptr
		&& PuzzleWidget->Implements<UI_MENUConnect>();
	if (IsWidgetCorrect) {
		PuzzleWidget->AddToViewport();
	}

	UFont* Font_Widget = PRESET->GetDefaultObject<UCPP_PRESET>()->Font_Widget;
	FLinearColor GoodGroundColor = PRESET->GetDefaultObject<UCPP_PRESET>()->GoodGroundColor;
	FLinearColor FreeGroundColor = PRESET->GetDefaultObject<UCPP_PRESET>()->FreeGroundColor;
	FLinearColor WrongGroundColor = PRESET->GetDefaultObject<UCPP_PRESET>()->WrongGroundColor;

	if (IsWidgetCorrect) {
		II_MENUConnect::Execute_ColorWidgetCPP(PuzzleWidget, Font_Widget, GoodGroundColor, FreeGroundColor, WrongGroundColor);
	}

	PlayerController->SetShowMouseCursor(true);
	FInputModeGameAndUI InputModeGameAndUI;
	InputModeGameAndUI.SetHideCursorDuringCapture(false);
	InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeGameAndUI);

	FTimerHandle TH_ScanPlate;
	GetWorldTimerManager().SetTimer(TH_ScanPlate, this, &ACPP_SlidePuzzle::ScanPlate, 0.1f, true);
	GetWorldTimerManager().SetTimer(TH_SlidePlate, this, &ACPP_SlidePuzzle::SlidePlate, 0.1f, true);
	GetWorldTimerManager().PauseTimer(TH_SlidePlate);
	GetWorldTimerManager().SetTimer(TH_HideBorders, this, &ACPP_SlidePuzzle::HideBorders, 0.01f, true);
	GetWorldTimerManager().PauseTimer(TH_HideBorders);

	SetArmLength();
}
void ACPP_SlidePuzzle::SetArmLength() {
	float AdjustedArmLength = 700.f * UniformScaleModifier;
	if (PlateNum == 2) {
		SpringArmComp->TargetArmLength = AdjustedArmLength;
		return;
	}
	for (uint8 ind = 2; ind <= MaxXY; ind++) {
		AdjustedArmLength -= 1.f / static_cast<float>(ind) * 155.f * UniformScaleModifier;
	}
	SpringArmComp->TargetArmLength = AdjustedArmLength;
	return;
}
void ACPP_SlidePuzzle::HideBorders() {
	float DeltaT = 0.01f;
	float InterpS = 4.f;

	if (HasGameEnded == false) {
		UGameplayStatics::PlaySound2D(this, SC_Win, 1.0f, 1.0f, 0.f, nullptr, this, true);
		HasGameEnded = true;
	}

	float FinalArmLength = 375.f * UniformScaleModifier;

	TArray<UStaticMeshComponent*> AllGrounds;
	GroundStatsMAP.GenerateKeyArray(AllGrounds);
	TArray<FGroundStats> AllGroundStats;
	GroundStatsMAP.GenerateValueArray(AllGroundStats);

	FVector ZeroG = FVector(0.f, 0.f, 0.f);
	for (uint8 ind = 0; ind < AllGrounds.Num(); ind++) {
		UStaticMeshComponent* OneGround = AllGrounds[ind];

		FVector GroundNewScale = FMath::VInterpTo(
			OneGround->GetRelativeScale3D(),
			ZeroG,
			DeltaT,
			InterpS
		);
		OneGround->SetRelativeScale3D(GroundNewScale);

		float SpringGoal = FMath::FInterpTo(
			SpringArmComp->TargetArmLength,
			FinalArmLength,
			DeltaT,
			0.25f
		);
		SpringArmComp->TargetArmLength = SpringGoal;

		UStaticMeshComponent* OnePlate = AllGroundStats[ind].OriginalPlate;
		if (OnePlate == nullptr) {
			continue;
		}
		FVector PlateNewScale = FMath::VInterpTo(
			OnePlate->GetRelativeScale3D(),
			GroundScale,
			DeltaT, InterpS
		);
		OnePlate->SetRelativeScale3D(PlateNewScale);
	}
	bool AreGroundsReady = AllGrounds.Last(0)->GetRelativeScale3D() == ZeroG;
	bool ArePlatesReady =
		AllGroundStats.Last(0).OriginalPlate == nullptr
		|| AllGroundStats.Last(0).OriginalPlate->GetRelativeScale3D() == GroundScale;
	bool IsSpringArmReady = FMath::IsNearlyEqual(SpringArmComp->TargetArmLength, FinalArmLength, 1.f);
	bool IsEverythingReady = AreGroundsReady && ArePlatesReady && IsSpringArmReady;
	if (IsEverythingReady == false) {
		return;
	}

	for (UTextRenderComponent* OneTR : NumbersARRAY) {
		OneTR->SetVisibility(false);
	}
	GetWorldTimerManager().PauseTimer(TH_HideBorders);
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////BEFORE EVERY MOVE//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
void ACPP_SlidePuzzle::MarkActivePlates() {
	float Ztrace = UniformScaleModifier * 10.f;
	for (UStaticMeshComponent* ActiveFG : FreeGroundARRAY) {
		FVector FGLocation = static_cast<FVector>(ActiveFG->Bounds.Origin);
		FQuat FGRotation = FQuat(ActiveFG->GetComponentRotation());

		TArray<FHitResult> Hit1;
		FCollisionShape Boxy1 = FCollisionShape::MakeBox(FVector(
			0.f,
			AdjustedPlateScale,
			Ztrace
		));
		GetWorld()->SweepMultiByChannel(Hit1,
			FGLocation, FGLocation, FGRotation,
			ECC_Visibility, Boxy1
		);

		TArray<FHitResult> Hit2;
		FCollisionShape Boxy2 = FCollisionShape::MakeBox(FVector(
			AdjustedPlateScale,
			0.f,
			Ztrace
		));
		GetWorld()->SweepMultiByChannel(Hit2,
			FGLocation, FGLocation, FGRotation,
			ECC_Visibility, Boxy2
		);

		TArray<FHitResult> PlatesAround;
		PlatesAround.Append(Hit1);
		PlatesAround.Append(Hit2);

		for (FHitResult ActiveHit : PlatesAround) {
			UStaticMeshComponent* PlateToAdd = RealPlate(ActiveHit);
			if (PlateToAdd != nullptr) {
				SelectablePlatesSET.Add(PlateToAdd);
			}
		}
	}
}
UStaticMeshComponent* ACPP_SlidePuzzle::RealPlate(FHitResult ActiveHit) {
	if (ActiveHit.bBlockingHit == false) {
		return nullptr;
	}
	UStaticMeshComponent* TracedPlate = Cast<UStaticMeshComponent>(ActiveHit.GetComponent());
	if (TracedPlate == nullptr) {
		return nullptr;
	}
	bool IsRealPlate =
		TracedPlate->ComponentTags.Num() == 4
		&& TracedPlate->ComponentTags[0] == PlateIdentifier
		&& TracedPlate->GetStaticMesh() == SM_Plate
		&& TracedPlate->GetOwner() == this;
	if (IsRealPlate == true) {
		return TracedPlate;
	}
	return nullptr;
}
void ACPP_SlidePuzzle::ScanPlate() {
	Scan_SelectPlate();
	bool KeepScanning =
		GrabMode == true
		&& SelectedPlate != nullptr;
	if (KeepScanning == false) {
		return;
	}
	Scan_AxisControl();
	Scan_GetAllPlateDirections();
}
void ACPP_SlidePuzzle::Scan_SelectPlate() {
	bool CanWeScan =
		GrabMode == false
		&& PlayerController != nullptr;
	if (CanWeScan == false) { return; }
	FHitResult Hit;

	//PlayerController->GetHitResultUnderCursorByChannel(
	//	UEngineTypes::ConvertToTraceType(ECC_Visibility),
	//	true,
	//	Hit
	//); // Doesn't work at huge scales (more them 100x)

	FVector TraceStart;
	FVector WorldDir;
	PlayerController->DeprojectMousePositionToWorld(TraceStart, WorldDir);
	FVector TraceEnd = WorldDir * 1000.f * UniformScaleModifier + TraceStart;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);

	SelectedPlate = RealPlate(Hit);
	bool DoSelectPlate =
		IsPlateMoving == false
		&& SelectedPlate != nullptr
		&& SelectablePlatesSET.Contains(SelectedPlate);
	if (DoSelectPlate == false) {
		SelectedPlate = nullptr;
		PlayerController->CurrentMouseCursor = EMouseCursor::Default;
		return;
	}
	else {
		PlayerController->CurrentMouseCursor = EMouseCursor::GrabHand;
		return;
	}
}
UStaticMeshComponent* ACPP_SlidePuzzle::GetGroundBelow() {
	FVector TraceStart = GetUnderPlateVector(MovingPlate);
	FVector TraceEnd = MovingPlate->GetUpVector() * (-10.f * UniformScaleModifier) + TraceStart;
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);
	UStaticMeshComponent* GroundBelow = Cast<UStaticMeshComponent>(Hit.Component);

	if (GroundBelow != nullptr) {
		return GroundBelow;
	}
	else { return nullptr; }
}
FVector ACPP_SlidePuzzle::GetUnderPlateVector(UStaticMeshComponent* PlateAbove) {
	FVector V1 = PlateAbove->Bounds.Origin;
	FVector V2 = PlateAbove->GetUpVector()
		* UniformScaleModifier
		* (1 / static_cast<float>(PlateNum));
	return V1 - V2;
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////DURING EVERY MOVE//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
void ACPP_SlidePuzzle::Scan_AxisControl() {
	float AxisX = PlayerInputComp->GetAxisKeyValue(EKeys::MouseX);
	float AxisY = PlayerInputComp->GetAxisKeyValue(EKeys::MouseY);

	if (AxisX == AxisY) {
		AxisPrecedence = EO_XisY;
		return;
	}

	float AxisXAbs = UKismetMathLibrary::Abs(AxisX);
	float AxisYAbs = UKismetMathLibrary::Abs(AxisY);
	if (AxisXAbs > AxisYAbs) {
		AxisPrecedence = (AxisX > 0.f) ? EO_RIGHT : EO_LEFT;
		return;
	}
	else {
		AxisPrecedence = (AxisY > 0.f) ? EO_UP : EO_DOWN;
		return;
	}
}
bool ACPP_SlidePuzzle::CheckFGDistance(UStaticMeshComponent* FGtoCheck) const {
	FVector V1 = SelectedPlate->GetComponentLocation();
	FVector V2 = FGtoCheck->GetComponentLocation();
	float ViDistance = FVector::Dist(V1, V2);
	float ErrorTolerance = UniformScaleModifier * BorderOffset * 2.f;
	return FMath::IsNearlyEqual(ViDistance, AdjustedPlateScale, ErrorTolerance);
}
void ACPP_SlidePuzzle::Scan_GetAllPlateDirections() {
	SidesToMoveSET.Empty(4);
	for (UStaticMeshComponent* FGtoCheck : FreeGroundARRAY) {
		bool KeepChecking = CheckFGDistance(FGtoCheck);
		if (KeepChecking == false) { continue; }

		float PlateX = FGtoCheck->GetRelativeLocation().X;
		float PlateY = FGtoCheck->GetRelativeLocation().Y;
		float GroundX = SelectedPlate->GetRelativeLocation().X;
		float GroundY = SelectedPlate->GetRelativeLocation().Y;

		bool IsYPrevails = FMath::IsNearlyEqual(PlateY, GroundY, BorderOffset * 2);
		E_AxisPrecedence DirectionToAdd;
		if (IsYPrevails == true) {
			DirectionToAdd = (PlateX > GroundX) ? EO_UP : EO_DOWN;
		}
		else {
			DirectionToAdd = (PlateY > GroundY) ? EO_RIGHT : EO_LEFT;
		}
		SidesToMoveSET.Add(DirectionToAdd);
	}
	StartPlateMovement();
}
void ACPP_SlidePuzzle::CheckOneGround(UStaticMeshComponent* GroundToCheck) {
	FGroundStats* Plates = GroundStatsMAP.Find(GroundToCheck);
	bool IsGroundFree = (Plates->CurrentPlate == nullptr);

	if (IsGroundFree == true) {
		GroundToCheck->SetMaterial(0, MID_Free);
		return;
	}

	bool ArePlatesTheSame = (Plates->CurrentPlate == Plates->OriginalPlate);

	if (ArePlatesTheSame == true) {
		GroundToCheck->SetMaterial(0, MID_Good);
		return;
	}
	else {
		GroundToCheck->SetMaterial(0, MID_Wrong);
		return;
	}
}
void ACPP_SlidePuzzle::AfterEveryMove() const {
	uint8 WrongGroundNum = TotalPlateCount - GoodGroundNum;

	bool IsWidgetCorrect =
		PuzzleWidget != nullptr
		&& PuzzleWidget->Implements<UI_MENUConnect>();

	if (IsWidgetCorrect == true) {
		II_MENUConnect::Execute_PlateStatsCPP(
			PuzzleWidget,
			GoodGroundNum,
			WrongGroundNum,
			TotalPlateCount
		);
	}
	if (TotalPlateCount != GoodGroundNum) {
		return;
	}
	GetWorldTimerManager().UnPauseTimer(TH_HideBorders);
	if (IsWidgetCorrect == true) {
		II_MENUConnect::Execute_EndgameCPP(PuzzleWidget);
	}
}
void ACPP_SlidePuzzle::IfGoodGround(UStaticMeshComponent* Ground, bool IsAfterMove) {
	uint8 GoodAdjustment = (IsAfterMove) ? 1 : -1;

	FGroundStats* Plates = GroundStatsMAP.Find(Ground);

	bool ArePlatesTheSame = (Plates->CurrentPlate == Plates->OriginalPlate);

	uint8 UpdatedNum = (ArePlatesTheSame) ? GoodGroundNum + GoodAdjustment : GoodGroundNum;

	GoodGroundNum = UpdatedNum;
}
void ACPP_SlidePuzzle::StartPlateMovement() {
	bool StartMove =
		AxisPrecedence != EO_XisY
		&& SelectedPlate != nullptr
		&& HasGameEnded == false;
	if (StartMove == false) { return; }
	GrabMode = false;
	E_AxisPrecedence AxisToMove = AxisPrecedence;

	for (uint8 ind = 0; ind < SidesToMoveSET.Num(); ind++) {
		TArray AllPossibleDirections = SidesToMoveSET.Array();
		E_AxisPrecedence DirectionToCheck = AllPossibleDirections[ind];
		if (DirectionToCheck == AxisToMove) { break; }
		if (ind == SidesToMoveSET.Num() - 1 && DirectionToCheck != AxisToMove) {
			return;
		}
	}

	float Correction = 0.f;
	switch (AxisToMove) {
	case EO_UP:
		Correction = PlateXYScale;
		break;
	case EO_DOWN:
		Correction = -PlateXYScale;
		break;
	default:
		Correction = 0.f;
		break;
	}
	float TargetX = SelectedPlate->GetRelativeLocation().X + Correction;

	switch (AxisToMove) {
	case EO_LEFT:
		Correction = -PlateXYScale;
		break;
	case EO_RIGHT:
		Correction = PlateXYScale;
		break;
	default:
		Correction = 0.f;
		break;
	}
	float TargetY = SelectedPlate->GetRelativeLocation().Y + Correction;

	PlateTarget = FVector(TargetX, TargetY, 1.0f);

	MovingPlate = SelectedPlate;
	IsPlateMoving = true;

	UStaticMeshComponent* DepartGround = GetGroundBelow();
	FreeGroundARRAY.Add(DepartGround);
	IfGoodGround(DepartGround, false);

	FGroundStats* NewFGPlates = GroundStatsMAP.Find(DepartGround);
	NewFGPlates->CurrentPlate = nullptr;

	DepartGround->SetMaterial(0, MID_Free);

	UGameplayStatics::PlaySound2D(this, SC_Move, 1.f, 1.f, 0.f, nullptr, this, true);
	GetWorldTimerManager().UnPauseTimer(TH_SlidePlate);
	return;
}
void ACPP_SlidePuzzle::SlidePlate() {
	if (MovingPlate == nullptr) { return; }

	FVector NewLocation = FMath::VInterpTo(
		MovingPlate->GetRelativeLocation(),
		PlateTarget,
		0.01,
		100.f
	);

	MovingPlate->SetRelativeLocation(NewLocation);

	bool IsTargetReached =
		FMath::IsNearlyEqual(MovingPlate->GetRelativeLocation().X, PlateTarget.X, 0.01f)
		&& FMath::IsNearlyEqual(MovingPlate->GetRelativeLocation().Y, PlateTarget.Y, 0.01f)
		&& FMath::IsNearlyEqual(MovingPlate->GetRelativeLocation().Z, PlateTarget.Z, 0.01f);
	if (IsTargetReached == false) { return; }

	UStaticMeshComponent* TagetGround = GetGroundBelow();
	FreeGroundARRAY.Remove(TagetGround);

	FGroundStats* GroundStats = GroundStatsMAP.Find(TagetGround);
	GroundStats->CurrentPlate = MovingPlate;

	IfGoodGround(TagetGround, true);
	CheckOneGround(TagetGround);

	GetWorldTimerManager().PauseTimer(TH_SlidePlate);

	MovingPlate = nullptr;
	IsPlateMoving = false;
	MarkActivePlates();

	AfterEveryMove();
}