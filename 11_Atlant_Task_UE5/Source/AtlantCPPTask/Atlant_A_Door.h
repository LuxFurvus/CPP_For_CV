// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Atlant_I_Interactable.h"
#include "Atlant_A_Door.generated.h"

UCLASS(Blueprintable)
class ATLANTCPPTASK_API AAtlant_A_Door final : public AActor, public IAtlant_I_Interactable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
	USceneComponent* DoorRoot = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
	UStaticMeshComponent* DoorBody = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USphereComponent* DestPoint = nullptr;

private:
	FTimerHandle DoorTimer;
	FVector StartPosition;
	FVector EndPosition;
	const float TotalMoveTime = 1.0f; // Move duration in seconds
	float ElapsedTime = 0.0f;

	UPROPERTY(Replicated)
	bool IsOpen = false;

	AAtlant_A_Door();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	void Interact(AActor* Caller) override;
	bool GetIsInteractable() const override { return !IsOpen; }
	void OpenDoor();

	UFUNCTION(Server, Unreliable)
	void SERVER_OpenDoor(const FVector NewPosition);
	void SERVER_OpenDoor_Implementation(const FVector NewPosition);

	UFUNCTION(NetMulticast, Unreliable)
	void MULTI_OpenDoor(const FVector NewPosition);
	void MULTI_OpenDoor_Implementation(const FVector NewPosition);
};
