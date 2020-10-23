// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "Blueprint/UserWidget.h"
#include "SPGameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API USPGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnResumeClicked();

	UFUNCTION()
		void OnReturnToTitleClicked();

	UFUNCTION()
		void OnRetryGameClicked();

protected:
	UPROPERTY()
		class UButton* ResumeButton;

	UPROPERTY()
		class UButton* ReturnToTitleButton;

	UPROPERTY()
		class UButton* RetryGameButton;
};
