// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameModeBase.h"
#include "SPPawn.h"
#include "SPPlayerController.h"

ASPGameModeBase::ASPGameModeBase()
{
	DefaultPawnClass = ASPPawn::StaticClass();
	PlayerControllerClass = ASPPlayerController::StaticClass();
}

void ASPGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	ABLOG(Warning, TEXT("PostLogin Begin"));

	Super::PostLogin(NewPlayer);

	ABLOG(Warning, TEXT("PostLogin End"));
}
