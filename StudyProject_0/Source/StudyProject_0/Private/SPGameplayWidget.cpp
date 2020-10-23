// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameplayWidget.h"
#include "SPPlayerController.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void USPGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));
	if (nullptr != ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &USPGameplayWidget::OnResumeClicked);
	}

	ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
	if (nullptr != ReturnToTitleButton)
	{
		ReturnToTitleButton->OnClicked.AddDynamic(this, &USPGameplayWidget::OnReturnToTitleClicked);
	}

	RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
	if (nullptr != RetryGameButton)
	{
		RetryGameButton->OnClicked.AddDynamic(this, &USPGameplayWidget::OnRetryGameClicked);
	}
}

void USPGameplayWidget::OnResumeClicked()
{
	auto SPPlayerController = Cast<ASPPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != SPPlayerController);

	RemoveFromParent();

	SPPlayerController->ChangeInputMode(true);
	SPPlayerController->SetPause(false);
}

void USPGameplayWidget::OnReturnToTitleClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Title"));
}

void USPGameplayWidget::OnRetryGameClicked()
{
	auto SPPlayerController = Cast<ASPPlayerController>(GetOwningPlayer());
	ABCHECK(nullptr != SPPlayerController);
	SPPlayerController->RestartLevel();
}
