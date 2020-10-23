// Fill out your copyright notice in the Description page of Project Settings.


#include "SPPlayerController.h"
#include "SPHUDWidget.h"
#include "SPPlayerState.h"
#include "SPCharacter.h"
#include "SPGameplayWidget.h"
#include "SPGameplayResultWidget.h"
#include "SPGameState.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ASPPlayerController::ASPPlayerController()
	: HUDWidget(nullptr)
{
	static ConstructorHelpers::FClassFinder<USPHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded())
		HUDWidgetClass = UI_HUD_C.Class;

	static ConstructorHelpers::FClassFinder<USPGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
	if (UI_MENU_C.Succeeded())
		MenuWidgetClass = UI_MENU_C.Class;

	static ConstructorHelpers::FClassFinder<USPGameplayResultWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT_C.Succeeded())
		ResultWidgetClass = UI_RESULT_C.Class;
}

void ASPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ChangeInputMode(true);

	HUDWidget = CreateWidget<USPHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ResultWidget = CreateWidget<USPGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

	SPPlayerState = Cast<ASPPlayerState>(PlayerState);
	ABCHECK(nullptr != SPPlayerState);
	HUDWidget->BindPlayerState(SPPlayerState);
	SPPlayerState->OnPlayerStateChanged.Broadcast();
}

void ASPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this, &ASPPlayerController::OnGamePause);
}

void ASPPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<USPGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true);
	ChangeInputMode(false);
}

void ASPPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void ASPPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

USPHUDWidget* ASPPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void ASPPlayerController::NPCKill(ASPCharacter* KilledNPC) const
{
	SPPlayerState->AddExp(KilledNPC->GetExp());
}

void ASPPlayerController::AddGameScore() const
{
	SPPlayerState->AddGameScore();
}

void ASPPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void ASPPlayerController::ShowResultUI()
{
	auto SPGameState = Cast<ASPGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(nullptr != SPGameState);
	ResultWidget->BindGameStae(SPGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}
