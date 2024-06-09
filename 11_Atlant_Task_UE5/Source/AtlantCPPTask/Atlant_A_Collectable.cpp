// Fill out your copyright notice in the Description page of Project Settings.

#include "Atlant_A_Collectable.h"
#include "E_ItemTypes.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

AAtlant_A_Collectable::AAtlant_A_Collectable()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RotationComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotationComponent"));;
}

void AAtlant_A_Collectable::BeginPlay()
{
	Super::BeginPlay();

	if (ItemOwnType == E_ItemTypes::NONE)
	{
		const E_ItemTypes MainItemType = static_cast<E_ItemTypes>(UKismetMathLibrary::RandomIntegerInRange(1, 3));

		TSubclassOf<class AAtlant_A_Collectable> SpawedClass = nullptr;

		switch (MainItemType)
		{
		case E_ItemTypes::BALL: SpawedClass = BallClass; break;
		case E_ItemTypes::CONE: SpawedClass = ConeClass; break;
		case E_ItemTypes::CUBE: SpawedClass = CubeClass; break;
		case E_ItemTypes::NONE: SpawedClass = nullptr; break;
		}

		const FTransform SpawnTransform{ FRotator::ZeroRotator, GetActorLocation() };

		SERVER_Spawn(SpawedClass, SpawnTransform);
		return;
	}

	if (!Body) return;

	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	if (!GameState) return;

	GameStateI = Cast<IAtlant_I_FromGameState>(GameState);
	if (!GameStateI) return;

	if (!IsRandomRotation) return;

	const float RandFloat1 = UKismetMathLibrary::RandomFloatInRange(0.0f, 180.0f);
	const float RandFloat2 = UKismetMathLibrary::RandomFloatInRange(0.0f, 180.0f);
	const float RandFloat3 = UKismetMathLibrary::RandomFloatInRange(0.0f, 180.0f);

	RotationComponent->RotationRate = FRotator(RandFloat1, RandFloat2, RandFloat3);
}

bool AAtlant_A_Collectable::GetIsInteractable() const
{
	if (!GameStateI) return false;
	return (ItemOwnType == GameStateI->GetItemType());
}

void AAtlant_A_Collectable::SERVER_Spawn_Implementation(TSubclassOf<class AAtlant_A_Collectable> SpawedClass, const FTransform SpawnTransform)
{
	GetWorld()->SpawnActor<AAtlant_A_Collectable>(SpawedClass, SpawnTransform);
	Destroy();
}

void AAtlant_A_Collectable::Interact(AActor* Caller)
{
	if (!GameStateI || (ItemOwnType != GameStateI->GetItemType())) return;

	GameStateI->AddItemCollected();
	Destroy();
}

