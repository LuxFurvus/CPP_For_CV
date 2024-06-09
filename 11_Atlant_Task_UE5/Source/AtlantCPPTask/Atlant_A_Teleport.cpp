// Fill out your copyright notice in the Description page of Project Settings.

#include "Atlant_A_Teleport.h"

// Sets default values
AAtlant_A_Teleport::AAtlant_A_Teleport()
{
	TeleportRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportRoot"));
	TeleportRoot->SetupAttachment(RootComponent);

	TeleportBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportBody"));
	TeleportBody->SetupAttachment(TeleportRoot);

	DestPoint = CreateDefaultSubobject<USphereComponent>(TEXT("DestPoint"));
	DestPoint->SetupAttachment(TeleportBody);

	bReplicates = true;
}

void AAtlant_A_Teleport::Interact(AActor* Caller)
{
    if(!OtherSide || !DestPoint) return;
	SERVER_Teleport(Caller, OtherSide->DestPoint->GetComponentLocation());
}

void AAtlant_A_Teleport::SERVER_Teleport_Implementation(AActor* Target, const FVector ExitPoint)
{
	Target->SetActorLocation(ExitPoint);
	MULTI_Teleport(Target, ExitPoint);
}

void AAtlant_A_Teleport::MULTI_Teleport_Implementation(AActor* Target, const FVector ExitPoint)
{
	Target->SetActorLocation(ExitPoint);
}
