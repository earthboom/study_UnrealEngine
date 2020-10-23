// Fill out your copyright notice in the Description page of Project Settings.


#include "SPCharacterWidget.h"
#include "SPCharacterStatComponent.h"
#include "Components/ProgressBar.h"

void USPCharacterWidget::BindCharacterStat(class USPCharacterStatComponent* NewCharacterStat)
{
	ABCHECK(nullptr != NewCharacterStat);

	CurrentCharacterStat = NewCharacterStat;

	NewCharacterStat->OnHPChanged.AddUObject(this, &USPCharacterWidget::UpdateHPWidget);
	//NewCharacterStat->OnHPChanged.AddLambda([this]()->void {
	//	if (CurrentCharacterStat.IsValid())
	//	{
	//		ABLOG(Warning, TEXT("HPRatio : %f"), CurrentCharacterStat->GetHPRatio());
	//	}
	//});
}

void USPCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void USPCharacterWidget::UpdateHPWidget()
{
	if (CurrentCharacterStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
		}
	}
}
