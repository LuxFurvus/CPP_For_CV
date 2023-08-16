// Copyright MangoFalls. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "ESP_Grabber.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BUILDINGESCAPE_API UESP_Grabber : public UActorComponent {
	GENERATED_BODY()
private:////////////////////////////////////////////
	UPhysicsHandleComponent* FisCom = nullptr;
	UInputComponent* InputComp = nullptr;
	FHitResult Hit;
	FVector StartG;
	FVector EndG;
	float GrabDistance = 200.f;
	UESP_Grabber() {
		PrimaryComponentTick.bCanEverTick = true;
	};

	void GrabSetup() {
		FisCom = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
		//if (FisCom) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "FisCom Found!!!");

		InputComp = GetOwner()->FindComponentByClass<UInputComponent>();
		if (InputComp) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "InputComp Found!!!");
			InputComp->BindAction("Grab", IE_Pressed, this, &UESP_Grabber::GRAB);
			InputComp->BindAction("Grab", IE_Released, this, &UESP_Grabber::DROP);
		}
	}

	void GRAB() {
		UPrimitiveComponent* HitComp = Hit.GetComponent();

		if (HitComp && FisCom && !IsHoldingItem) {
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, "GRAB pressed");
			IsHoldingItem = true;

			FisCom->GrabComponentAtLocation(
				HitComp,
				NAME_None,
				HitComp->GetOwner()->GetActorLocation()
			);
		}
	};

	void DROP() {
		if (FisCom && IsHoldingItem) {
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, "GRAB released");
			FisCom->ReleaseComponent();
			IsHoldingItem = false;
		}
	};

	void GetStartEnd(FVector& S, FVector& E) {
		FVector PlayerVecG;
		FRotator PlayerRotG;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerVecG, PlayerRotG);
		S = PlayerVecG;
		E = S + (PlayerRotG.Vector() * GrabDistance);
	}

	void LineTRACE() {
		//FCollisionQueryParams TracePar0;
		FCollisionQueryParams TracePar(FName(TEXT("")), false, GetOwner());
		bool WasHit = GetWorld()->LineTraceSingleByObjectType(Hit, StartG, EndG, ECC_PhysicsBody, TracePar);

		//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, .5f, 0, .0f);
		FQuat z;
		if (WasHit) {
			DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), z, FColor::Emerald, false, .5f);
		}
	}

protected:////////////////////////////////////////////
	virtual void BeginPlay() override {
		Super::BeginPlay();
		GrabSetup();
	}
public:////////////////////////////////////////////
	bool IsHoldingItem = false;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override {
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		GetStartEnd(StartG, EndG);

		if (!IsHoldingItem) {
			LineTRACE();
			AActor* Hitted = Hit.GetActor();
			if (Hit.bBlockingHit) {
				GEngine->AddOnScreenDebugMessage(1, .0f, FColor::Magenta, *(Hitted->GetName()));
				GEngine->AddOnScreenDebugMessage(2, .0f, FColor::Green, *(Hitted->GetActorLabel()));
			}
		}

		if (FisCom->GrabbedComponent) {
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, "HOLDING!!!");
			FisCom->SetTargetLocation(EndG);
		}
	}
};