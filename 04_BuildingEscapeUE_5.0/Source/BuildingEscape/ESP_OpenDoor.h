// Copyright MangoFalls. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "Components/PrimitiveComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ESP_Grabber.h"
//////////////////////////////////////////////////////////////////////////
#include "ESP_OpenDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenRequest);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_API UESP_OpenDoor : public UActorComponent {
	GENERATED_BODY()
private:///////////////////////////////////////////
	int32 TotalMassOnPlate = 0;
	int32 NumActorsOnPlateLast = 0;
	UTextRenderComponent* TextRender = nullptr;
	UESP_Grabber* PlayerGrab = nullptr;
	ACharacter* CHARA = nullptr;

	FRotator DoorInitialRotation;

	bool IsDoorClosed = true;
	float DoorTarget;
	float DoorBase;
	float DoorOpenRot;
	float DoorSpeed = .5f;
	float DoorWasOpenAt;
	float DoorYawNow;
	float TimeNow;

	UPROPERTY(EditAnywhere)		ATextRenderActor* MassCounter = nullptr;
	UPROPERTY(EditAnywhere)		ATriggerVolume* PressPlate = nullptr;

	void DoorControl() {
		if (TotalMassOnPlate / 50 >= CriticalMass && DoorYawNow <= DoorOpenRot) {
			DoorTarget += DoorSpeed;
			GetOwner()->SetActorRotation(FRotator(.0f, DoorTarget, .0f));
			MassCounter->GetTextRender()->SetTextRenderColor(FColor::Emerald);
			GEngine->AddOnScreenDebugMessage(-1, .0f, FColor::Cyan, "OPEN...");
		}
		if (TotalMassOnPlate / 50 >= CriticalMass) {
			DoorWasOpenAt = TimeNow;
		}

		if (TotalMassOnPlate / 50 < CriticalMass
			&& DoorYawNow >= DoorBase
			&& TimeNow - DoorWasOpenAt > 1.f)
		{
			DoorTarget -= DoorSpeed;
			GetOwner()->SetActorRotation(FRotator(.0f, DoorTarget, .0f));
			MassCounter->GetTextRender()->SetTextRenderColor(FColor::Red);
			GEngine->AddOnScreenDebugMessage(-1, .0f, FColor::Cyan, "CLOSE...");
		}
	}

	void DoorSound() {
		if (TimeNow > 5.f && IsDoorClosed && DoorYawNow != DoorBase - DoorSpeed) {
			IsDoorClosed = false;
			OnDoorOpen.Broadcast();
		}
		if (TimeNow > 5.f && !IsDoorClosed && DoorYawNow == DoorBase - DoorSpeed) {
			IsDoorClosed = true;
			OnDoorClose.Broadcast();
		}
	}

	void WeightMass() {
		TSet<AActor*> OOO;
		if (!PressPlate) {
			GEngine->AddOnScreenDebugMessage(-1, 900.0f, FColor::Red, ("PressPlate NOT FOUND!!!"));
			return;
		}
		PressPlate->GetOverlappingActors(OOO);
		int32 NumActorsOnPlateNow = OOO.Num();

		if (NumActorsOnPlateNow != NumActorsOnPlateLast && !(PlayerGrab->IsHoldingItem)) {
			TotalMassOnPlate = 0;
			for (auto& i : OOO) {
				UPrimitiveComponent* SM = i->FindComponentByClass<UPrimitiveComponent>();
				if (SM) TotalMassOnPlate += SM->GetMass();
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, .0f, FColor::Blue, FString::Printf(TEXT("Total Mass: %d"), TotalMassOnPlate));
		return;
	}

	void SetMassCounter() {
		if (MassCounter) TextRender = MassCounter->GetTextRender();
		else {
			GEngine->AddOnScreenDebugMessage(-1, 900.0f, FColor::Red, ("MassCounter NOT FOUND!!!"));
			return;
		}
		FText ToPrint = FText::FromString(FString::SanitizeFloat(TotalMassOnPlate / 50));
		if (TextRender) TextRender->SetText(ToPrint);
	}

protected:///////////////////////////////////////////
	virtual void BeginPlay() override {
		Super::BeginPlay();
		PlayerGrab = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
			->FindComponentByClass<UESP_Grabber>();
		DoorBase = GetOwner()->GetActorRotation().Yaw;
		DoorTarget = DoorBase;
		DoorOpenRot = DoorBase + OpenAngle;
	};
public:///////////////////////////////////////////
	UESP_OpenDoor() {
		PrimaryComponentTick.bCanEverTick = true;
	}

	UPROPERTY(BlueprintAssignable) FOnOpenRequest OnDoorOpen;
	UPROPERTY(BlueprintAssignable) FOnOpenRequest OnDoorClose;

	UPROPERTY(EditAnywhere) float CriticalMass = 10.f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 90.0f))
		float OpenAngle = 90.0f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};

/////////////////////////////////////////////////
void UESP_OpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeNow = GetWorld()->GetTimeSeconds();
	DoorYawNow = GetOwner()->GetActorRotation().Yaw;

	WeightMass();
	SetMassCounter();
	DoorControl();
	DoorSound();
}