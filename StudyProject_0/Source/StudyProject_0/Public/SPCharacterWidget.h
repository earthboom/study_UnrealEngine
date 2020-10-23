// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "Blueprint/UserWidget.h"
#include "SPCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API USPCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindCharacterStat(class USPCharacterStatComponent* NewCharacterStat);
	
protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

private:
	TWeakObjectPtr<class USPCharacterStatComponent> CurrentCharacterStat;

	UPROPERTY()
	class UProgressBar* HPProgressBar;
};
