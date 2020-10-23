// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPGameplayWidget.h"
#include "SPGameplayResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API USPGameplayResultWidget : public USPGameplayWidget
{
	GENERATED_BODY()
	
public:
	void BindGameStae(class ASPGameState* GameState);

protected:
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<class ASPGameState> CurrentGameState;
};
