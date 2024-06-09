// Fill out your copyright notice in the Description page of Project Settings.


#include "Atlant_A_Door.h"

// Sets default values
AAtlant_A_Door::AAtlant_A_Door()
{
	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	DoorRoot->SetupAttachment(RootComponent);

	DoorBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorBody"));
	DoorBody->SetupAttachment(DoorRoot);

	DestPoint = CreateDefaultSubobject<USphereComponent>(TEXT("DestPoint"));
	DestPoint->SetupAttachment(DoorBody);

	bReplicates = true;
}

void AAtlant_A_Door::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAtlant_A_Door, IsOpen);
}

void AAtlant_A_Door::BeginPlay()
{
	Super::BeginPlay();

	StartPosition = GetActorLocation();
	EndPosition = DestPoint->GetComponentLocation();
	GetWorldTimerManager().SetTimer(DoorTimer, this, &AAtlant_A_Door::OpenDoor, 0.01f, true);
	GetWorldTimerManager().PauseTimer(DoorTimer);
}

void AAtlant_A_Door::Interact(AActor* Caller)
{
	if (IsOpen) return;
	GetWorldTimerManager().UnPauseTimer(DoorTimer);
	IsOpen = true;
}

void AAtlant_A_Door::OpenDoor()
{
	ElapsedTime += 0.01f;
	if (ElapsedTime <= TotalMoveTime)
	{
		const float Alpha = ElapsedTime / TotalMoveTime;
		const FVector NewPosition = FMath::Lerp(StartPosition, EndPosition, Alpha);
		SERVER_OpenDoor(NewPosition);
	}
	else
	{
		// Movement is complete
		SERVER_OpenDoor(EndPosition);
		GetWorldTimerManager().ClearTimer(DoorTimer);
	}
}

void AAtlant_A_Door::SERVER_OpenDoor_Implementation(const FVector NewPosition)
{
	SetActorLocation(NewPosition);
	MULTI_OpenDoor(NewPosition);
}

void AAtlant_A_Door::MULTI_OpenDoor_Implementation(const FVector NewPosition)
{
	SetActorLocation(NewPosition);
}
