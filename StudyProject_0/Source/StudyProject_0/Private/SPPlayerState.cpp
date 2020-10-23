// Fill out your copyright notice in the Description page of Project Settings.


#include "SPPlayerState.h"
#include "SPGameInstance.h"
#include "SPSaveGame.h"

#include "Kismet/GameplayStatics.h"

ASPPlayerState::ASPPlayerState()
	: CharacterLevel(1), GameScore(0), Exp(0)
	, GameHighScore(0), CharacterIndex(0)
	, SaveSlotName(TEXT("Player1"))
{

}

int32 ASPPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 ASPPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

int32 ASPPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 ASPPlayerState::GetCharacterIndex() const
{
	return CharacterIndex;
}

float ASPPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.0f;

	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current: %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool ASPPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	Exp = Exp + IncomeExp;
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(CharacterLevel + 1);
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();
	SavePlayerData();
	return DidLevelUp;
}

void ASPPlayerState::AddGameScore()
{
	++GameScore;
	if (GameScore >= GameHighScore)
		GameHighScore = GameScore;

	OnPlayerStateChanged.Broadcast();
	SavePlayerData();
}

void ASPPlayerState::InitPlayerData()
{
	auto SPSaveGame = Cast<USPSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (nullptr == SPSaveGame)
		SPSaveGame = GetMutableDefault<USPSaveGame>();

	SetPlayerName(SPSaveGame->PlayerName);
	SetCharacterLevel(SPSaveGame->Level);
	GameScore = 0;
	GameHighScore = SPSaveGame->HighScore;
	Exp = SPSaveGame->Exp;
	CharacterIndex = SPSaveGame->CharacterIndex;

	SavePlayerData();
}

void ASPPlayerState::SavePlayerData()
{
	USPSaveGame* NewPlayerData = NewObject<USPSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;
	NewPlayerData->CharacterIndex = CharacterIndex;

	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
	{
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}

void ASPPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto SPGameInstance = Cast<USPGameInstance>(GetGameInstance());
	ABCHECK(nullptr != SPGameInstance);

	CurrentStatData = SPGameInstance->GetSPCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
