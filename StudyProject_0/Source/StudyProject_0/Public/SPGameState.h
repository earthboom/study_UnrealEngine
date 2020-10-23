// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "GameFramework/GameStateBase.h"
#include "SPGameState.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API ASPGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ASPGameState();

public:
	int32 GetTotalGameScore() const;
	void AddGameScore();
	void SetGameCleared();
	bool IsGameCleared() const;

private:
	UPROPERTY(Transient)
		int32 TotalGameScore;

	UPROPERTY(Transient)
		bool bGameCleared;
};
