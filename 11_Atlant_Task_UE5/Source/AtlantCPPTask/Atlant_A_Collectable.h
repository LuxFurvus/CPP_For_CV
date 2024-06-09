// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "E_ItemTypes.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Atlant_I_Interactable.h"
#include "Atlant_I_FromGameState.h"
#include "Atlant_A_Collectable.generated.h"

UCLASS(Blueprintable, meta = (PrioritizeCategories = "Spawnables"))
class ATLANTCPPTASK_API AAtlant_A_Collectable final : public AActor, public IAtlant_I_Interactable
{
	GENERATED_BODY()
public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables")
	E_ItemTypes ItemOwnType = E_ItemTypes::NONE;

	//UPROPERTY(Replicated)
	//E_ItemTypes TrueItemType = E_ItemTypes::NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables")
	bool IsRandomRotation = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables", meta = (EditCondition = "ItemOwnType == E_ItemTypes::NONE", EditConditionHides))
	TSubclassOf<class AAtlant_A_Collectable> BallClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables", meta = (EditCondition = "ItemOwnType == E_ItemTypes::NONE", EditConditionHides))
	TSubclassOf<class AAtlant_A_Collectable> ConeClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables", meta = (EditCondition = "ItemOwnType == E_ItemTypes::NONE", EditConditionHides))
	TSubclassOf<class AAtlant_A_Collectable> CubeClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables", AdvancedDisplay)
	UStaticMeshComponent* Body = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawnables", AdvancedDisplay)
	URotatingMovementComponent* RotationComponent = nullptr;

private:
	IAtlant_I_FromGameState* GameStateI = nullptr;

	AAtlant_A_Collectable();
	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void BeginPlay() override;
	void Interact(AActor* Caller) override;
	bool GetIsInteractable() const override;

	UFUNCTION(Server, Unreliable)
	void SERVER_Spawn(TSubclassOf<class AAtlant_A_Collectable> SpawedClass, const FTransform SpawnTransform);
	void SERVER_Spawn_Implementation(TSubclassOf<class AAtlant_A_Collectable> SpawedClass, const FTransform SpawnTransform);

	//UFUNCTION(NetMulticast, Unreliable)
	//void MILTI_Interact();
	//void MILTI_Interact_Implementation();
};
