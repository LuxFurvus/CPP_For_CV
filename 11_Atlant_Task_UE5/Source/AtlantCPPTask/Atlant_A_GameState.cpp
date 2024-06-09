// Fill out your copyright notice in the Description page of Project Settings.

#include "Atlant_A_GameState.h"
#include "Atlant_I_FromGameMode.h"
#include "Atlant_I_ForPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMesh.h"
//UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "Lo=lo", 1, 0, FLinearColor::Green, 2.0f);
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("CurrentItemType = %d"), static_cast<int32>(CurrentItemType)), true, true, FLinearColor::Blue, 9.0f);

AAtlant_A_GameState::AAtlant_A_GameState()
{
	bReplicates = true;
}

void AAtlant_A_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAtlant_A_GameState, ItemCollected); 
	DOREPLIFETIME(AAtlant_A_GameState, CurrentItemType);
	DOREPLIFETIME(AAtlant_A_GameState, MenuWidgetClass);
	DOREPLIFETIME(AAtlant_A_GameState, CollectNotifySound);
	DOREPLIFETIME(AAtlant_A_GameState, TimeBeforeEnd);
	DOREPLIFETIME(AAtlant_A_GameState, TimeAfterEnd);
	DOREPLIFETIME(AAtlant_A_GameState, ItemTotal);
}

void AAtlant_A_GameState::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(StateTimer, this, &AAtlant_A_GameState::DoCountdown, 0.1f, true);

	if (!AuthorityGameMode) return;

	IAtlant_I_FromGameMode* GameModeI = Cast<IAtlant_I_FromGameMode>(AuthorityGameMode);
	if (!GameModeI) return;

	CurrentItemType = GameModeI->GetItemType();
	MenuWidgetClass = GameModeI->GetMenuWidgetClass();
	CollectNotifySound = GameModeI->GetNotifySound();

	TimeBeforeEnd = GameModeI->GetTimeBeforeEnd();
	TimeAfterEnd = GameModeI->GetTimeAfterEnd();
	ItemTotal = GameModeI->GetItemTotal();
}

void AAtlant_A_GameState::OnRep_ItemCollected()
{
	for (APlayerState* Player : PlayerArray)
	{
		IAtlant_I_ForPlayerController* Controller = Cast<IAtlant_I_ForPlayerController>(Player->GetPawn()->GetController());
		if (!Controller) continue;

		Controller->OnCollectItem();
		MULTI_NotifyPlayers();
	}
}

void AAtlant_A_GameState::MULTI_NotifyPlayers_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), CollectNotifySound);
}

void AAtlant_A_GameState::MULTI_QuitAllPlayers_Implementation()
{
	for (APlayerState* Player : PlayerArray)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), Player->GetPlayerController(), EQuitPreference::Quit, true);
	}
}
void AAtlant_A_GameState::MULTI_DoAscension_Implementation()
{
	for (APlayerState* Player : PlayerArray)
	{
		IsEnd = true;
		USkeletalMeshComponent* PlayerSkelet = Player->GetPawn()->GetComponentByClass<USkeletalMeshComponent>();

		if (!PlayerSkelet) continue;

		PlayerSkelet->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		PlayerSkelet->SetSimulatePhysics(true);
		PlayerSkelet->SetEnableGravity(false);
		PlayerSkelet->AddImpulse(FVector(0.0f, 0.0f, 3000.0f), NAME_None, true);
		PlayerSkelet->AddForce(FVector(0.0f, 0.0f, 3000.0f), NAME_None, true);
	}
}

void AAtlant_A_GameState::DoCountdown()
{
	const int32 CurrentTime = static_cast<int32>(GetServerWorldTimeSeconds());

	switch (SessionStatus)
	{
	case E_SessionState::MAIN_PART:
	{
		TimeLeft = TimeBeforeEnd - CurrentTime;
		GameEndMoment = CurrentTime;

		if (ItemCollected >= ItemTotal)
		{
			SessionStatus = E_SessionState::VICTORY;
			IsEnd = true;
			IsWin = true;
			return;
		}

		if (TimeLeft < 0)
		{
			SessionStatus = E_SessionState::DEFEAT;
			return;
		}

		return;
	}
	case E_SessionState::VICTORY:
	{
		TimeLeft = TimeAfterEnd + GameEndMoment - CurrentTime;

		if (TimeLeft < 0)
		{
			MULTI_QuitAllPlayers();
			return;
		}
		return;
	}
	case E_SessionState::DEFEAT:
	{
		TimeLeft = TimeAfterEnd + GameEndMoment - CurrentTime;

		if (TimeLeft < 0)
		{
			MULTI_QuitAllPlayers();
			return;
		}

		if (IsEnd) return;
		MULTI_DoAscension();
	}
	}
}