// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Atlant_I_ForInteractComp.h"
#include "Atlant_I_Interactable.h"
#include "Atlant_I_FromGameState.h"
#include "Atlant_I_ForPlayerController.h"
#include "Atlant_U_InteractComponent.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class ATLANTCPPTASK_API UAtlant_U_InteractComponent : public UActorComponent, public IAtlant_I_ForInteractComp
{
	GENERATED_BODY()
	FTimerHandle SearchTimer;
	AActor* Owner = nullptr;
	AActor* HitActor = nullptr;
	UCameraComponent* CameraComponent = nullptr;
	IAtlant_I_FromGameState* GameStateI = nullptr;
	IAtlant_I_ForPlayerController* PlayerControllerI = nullptr;

	virtual void BeginPlay() override;
	void SearchForItems();
	void DrawBoxTrace(const bool bHit, const FVector CameraLocation, const FVector EndLocation, const FVector BoxHalfSize) const;
	UFUNCTION(Server, Unreliable)
	void SERVER_SendInteraction(AActor* Interactor, AActor* Target);
	void SERVER_SendInteraction_Implementation(AActor* Interactor, AActor* Target);
public:
	void RunInteraction() override;		
};

