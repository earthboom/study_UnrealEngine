// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "GameFramework/GameModeBase.h"
#include "SPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API ASPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASPGameModeBase();

	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AddScore(class ASPPlayerController* ScoredPlayer);
	int32 GetScore() const;

private:
	UPROPERTY()
		class ASPGameState* SPGameState;

	UPROPERTY()
		int32 ScoreToClear;
};
