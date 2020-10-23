// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameModeBase.h"
#include "SPPawn.h"
#include "SPCharacter.h"
#include "SPPlayerController.h"
#include "SPPlayerState.h"
#include "SPGameState.h"

ASPGameModeBase::ASPGameModeBase()
	: ScoreToClear(2)
{
	//DefaultPawnClass = ASPPawn::StaticClass();
	DefaultPawnClass = ASPCharacter::StaticClass();
	PlayerControllerClass = ASPPlayerController::StaticClass();
	PlayerStateClass = ASPPlayerState::StaticClass();
	GameStateClass = ASPGameState::StaticClass();
}

void ASPGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SPGameState = Cast<ASPGameState>(GameState);
}

void ASPGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto SPPlayerState = Cast<ASPPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != SPPlayerState);
	SPPlayerState->InitPlayerData();
}

void ASPGameModeBase::AddScore(ASPPlayerController* ScoredPlayer)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto SPPlayerController = Cast<ASPPlayerController>(It->Get());
		if ((nullptr != SPPlayerController) && (ScoredPlayer == SPPlayerController))
		{
			SPPlayerController->AddGameScore();
			break;
		}
	}

	SPGameState->AddGameScore();

	if (GetScore() >= ScoreToClear)
	{
		SPGameState->SetGameCleared();

		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			const auto SPPlayerContorller = Cast<ASPPlayerController>(It->Get());
			if (nullptr != SPPlayerContorller)
			{
				SPPlayerContorller->ShowResultUI();
			}
		}
	}
}

int32 ASPGameModeBase::GetScore() const
{
	return SPGameState->GetTotalGameScore();
}
