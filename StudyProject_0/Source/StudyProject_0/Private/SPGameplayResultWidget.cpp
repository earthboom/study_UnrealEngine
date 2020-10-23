// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameplayResultWidget.h"
#include "SPGameState.h"

#include "Components/TextBlock.h"

void USPGameplayResultWidget::BindGameStae(class ASPGameState* GameState)
{
	ABCHECK(nullptr != GameState);
	CurrentGameState = GameState;
}

void USPGameplayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABCHECK(CurrentGameState.IsValid());

	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	ABCHECK(nullptr != Result);
	Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed")));

	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	ABCHECK(nullptr != TotalScore);
	TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
}