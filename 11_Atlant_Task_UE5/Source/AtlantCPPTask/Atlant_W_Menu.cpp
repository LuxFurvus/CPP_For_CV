// Fill out your copyright notice in the Description page of Project Settings.

#include "Atlant_W_Menu.h"
#include "Atlant_I_FromGameState.h"
#include "E_ItemTypes.h"
#include "Kismet/GameplayStatics.h"

void UAtlant_W_Menu::NativeConstruct()
{
    Super::NativeConstruct();

    AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	if (!GameState) return;

    GameStateI = Cast<IAtlant_I_FromGameState>(GameState);
	if (!GameStateI) return;

    switch (GameStateI->GetItemType())
    {
    case E_ItemTypes::NONE: TX_ItemType->SetText(FText::FromString(TEXT("NONE!"))); break;
    case E_ItemTypes::BALL: TX_ItemType->SetText(FText::FromString(TEXT("Balls"))); break;
    case E_ItemTypes::CONE: TX_ItemType->SetText(FText::FromString(TEXT("Cones"))); break;
    case E_ItemTypes::CUBE: TX_ItemType->SetText(FText::FromString(TEXT("Cubes"))); break;
    }

    TX_ItemCollected->SetText(FText::AsNumber(GameStateI->GetItemCollected()));

    TX_ItemTotal->SetText(FText::AsNumber(GameStateI->GetItemTotal()));

    GetWorld()->GetTimerManager().SetTimer(HUDTimer, this, &UAtlant_W_Menu::WidgetUpdater, 0.05f, true);
}

void UAtlant_W_Menu::WidgetUpdater()
{
    if (!GameStateI) return;

    TX_TimeLeft->SetText(FText::AsNumber(GameStateI->GetTimeLeft()));

    if (!GameStateI->GetIsEnd()) return;

    IsEnd = true;

    if(GameStateI->GetIsWin())
	{
        TX_FinalResult->SetText(FText::FromString(TEXT("YOU WIN")));
    }
    else
    {
        TX_FinalResult->SetText(FText::FromString(TEXT("YOU LOSE")));
    }
    
    TX_FinalResult->SetVisibility(ESlateVisibility::Visible);

    TX_Hint->SetVisibility(ESlateVisibility::Visible);
    TX_Hint->SetText(FText::FromString(TEXT("to Quit press(G)")));

    TX_TimeLeft->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
}

void UAtlant_W_Menu::OnCollection()
{
    if (!GameStateI) return;
    TX_ItemCollected->SetText(FText::AsNumber(GameStateI->GetItemCollected()));    
}

void UAtlant_W_Menu::ShowHideHint(bool IsInteractable)
{
    if (IsInteractable)
    {
        TX_Hint->SetVisibility(ESlateVisibility::Visible);
        return;
    }
    TX_Hint->SetVisibility(ESlateVisibility::Hidden);    
}
