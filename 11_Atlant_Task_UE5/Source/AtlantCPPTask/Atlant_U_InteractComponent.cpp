// Fill out your copyright notice in the Description page of Project Settings.

#include "Atlant_U_InteractComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Atlant_I_FromGameState.h"
#include "Atlant_I_Interactable.h"
#include "Atlant_I_ForPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UAtlant_U_InteractComponent::BeginPlay()
{
	Super::BeginPlay();

	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	if (!GameState) return;

	GameStateI = Cast<IAtlant_I_FromGameState>(GameState);
    Owner = GetOwner();
    CameraComponent = Owner->FindComponentByClass<UCameraComponent>();

    GetWorld()->GetTimerManager().SetTimer(SearchTimer, this, &UAtlant_U_InteractComponent::SearchForItems, 0.05f, true);

    APawn* PlayerPawn = Cast<APawn>(Owner);
	if (!PlayerPawn) return;

    AController* PlayerController = PlayerPawn->GetController();
	if (!PlayerController) return;

    PlayerControllerI = Cast<IAtlant_I_ForPlayerController>(PlayerController);
}

void UAtlant_U_InteractComponent::RunInteraction()
{
    if (!Cast<IAtlant_I_Interactable>(HitActor)) return;

    SERVER_SendInteraction(Owner, HitActor);
}

void UAtlant_U_InteractComponent::SERVER_SendInteraction_Implementation(AActor* Interactor, AActor* Target)
{
    if (!Interactor || !Target) return;

    IAtlant_I_Interactable* InteractableObj = Cast<IAtlant_I_Interactable>(Target);

    InteractableObj->Interact(Interactor);
}

void UAtlant_U_InteractComponent::SearchForItems()
{
    const bool AreElementsValid
    {
        Owner
        && CameraComponent
        && GameStateI
        && !GameStateI->GetIsEnd()
    };
    if (!AreElementsValid) return;

    const FVector CameraLocation = CameraComponent->GetComponentLocation();
    const FVector ForwardVector = CameraComponent->GetForwardVector();
    const FVector EndLocation = CameraLocation + (ForwardVector * 500.0f);
    const FVector BoxHalfSize = FVector(25.0f, 25.0f, 25.0f); // Define a long rectangle

    // Perform the Box Trace
    FHitResult OutHit;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->SweepSingleByChannel(
        OutHit,
        CameraLocation,
        EndLocation,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeBox(BoxHalfSize),
        CollisionParams
    );

    HitActor = OutHit.GetActor();

    if (!PlayerControllerI) return;

    IAtlant_I_Interactable* SomeItem = Cast<IAtlant_I_Interactable>(HitActor);

    const bool IsItemCollectable = bHit && SomeItem && SomeItem->GetIsInteractable();

    PlayerControllerI->ShowInteractHint(IsItemCollectable);

    //DrawBoxTrace(bHit, CameraLocation, EndLocation, BoxHalfSize);
}

void UAtlant_U_InteractComponent::DrawBoxTrace(const bool bHit, const FVector CameraLocation, const FVector EndLocation, const FVector BoxHalfSize) const
{
    const FColor TraceColor = bHit ? FColor::Red : FColor::Green;

    // Calculate the center of the box
    FVector BoxCenter = (CameraLocation + EndLocation) * 0.5f;

    // Calculate the rotation of the box
    FRotator BoxRotation = FRotationMatrix::MakeFromX(EndLocation - CameraLocation).Rotator();

    DrawDebugBox(
        GetWorld(),
        BoxCenter,
        BoxHalfSize,
        BoxRotation.Quaternion(),
        TraceColor,
        true, // bPersistentLines
        0.1f, // LifeTime
        0, // DepthPriority
        0.1f // Thickness
    );
}

