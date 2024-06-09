// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Atlant_I_Interactable.h"
#include "Atlant_A_Teleport.generated.h"

UCLASS(Blueprintable)
class ATLANTCPPTASK_API AAtlant_A_Teleport final : public AActor, public IAtlant_I_Interactable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport", AdvancedDisplay)
	USceneComponent* TeleportRoot = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Teleport", AdvancedDisplay)
	USceneComponent* TeleportBody = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport", AdvancedDisplay)
	USphereComponent* DestPoint = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Teleport")
	AAtlant_A_Teleport* OtherSide = nullptr;

private:
	AAtlant_A_Teleport();

	UFUNCTION(Server, Unreliable)
	void SERVER_Teleport(AActor* Target, const FVector ExitPoint);
	void SERVER_Teleport_Implementation(AActor* Target, const FVector ExitPoint);

	UFUNCTION(NetMulticast, Unreliable)
	void MULTI_Teleport(AActor* Target, const FVector ExitPoint);
	void MULTI_Teleport_Implementation(AActor* Target, const FVector ExitPoint);

	void Interact(AActor* Caller) override;
	bool GetIsInteractable() const override { return (OtherSide != nullptr); }
};
