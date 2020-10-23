// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "GameFramework/SaveGame.h"
#include "SPSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API USPSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USPSaveGame();

	UPROPERTY()
		int32 Level;

	UPROPERTY()
		int32 Exp;

	UPROPERTY()
		FString PlayerName;

	UPROPERTY()
		int32 HighScore;

	UPROPERTY()
		int32 CharacterIndex;
};
