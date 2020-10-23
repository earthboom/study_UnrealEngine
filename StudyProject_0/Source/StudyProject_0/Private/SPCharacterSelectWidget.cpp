// Fill out your copyright notice in the Description page of Project Settings.


#include "SPCharacterSelectWidget.h"
#include "SPCharacterSetting.h"
#include "SPGameInstance.h"
#include "SPSaveGame.h"
#include "SPPlayerState.h"

#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void USPCharacterSelectWidget::NextCharacter(bool bForward)
{
	bForward ? ++CurrentIndex : --CurrentIndex;

	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;

	auto CharacterSetting = GetDefault<USPCharacterSetting>();
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];

	auto SPGameInstance = GetWorld()->GetGameInstance<USPGameInstance>();
	ABCHECK(nullptr != SPGameInstance);
	ABCHECK(TargetComponent.IsValid());

	USkeletalMesh* Asset = SPGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
	if (nullptr != Asset)
	{
		TargetComponent->SetSkeletalMesh(Asset);
	}
}

void USPCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;
	auto CharacterSetting = GetDefault<USPCharacterSetting>();
	MaxIndex = CharacterSetting->CharacterAssets.Num();

	for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It)
	{
		TargetComponent = It->GetSkeletalMeshComponent();
		break;
	}

	PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
	ABCHECK(nullptr != PrevButton);

	NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
	ABCHECK(nullptr != NextButton);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
	ABCHECK(nullptr != TextBox);

	ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
	ABCHECK(nullptr != ConfirmButton);

	PrevButton->OnClicked.AddDynamic(this, &USPCharacterSelectWidget::OnPrevClicked);
	NextButton->OnClicked.AddDynamic(this, &USPCharacterSelectWidget::OnNextClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &USPCharacterSelectWidget::OnConfirmClicked);
}

void USPCharacterSelectWidget::OnPrevClicked()
{
	NextCharacter(false);
}

void USPCharacterSelectWidget::OnNextClicked()
{
	NextCharacter(true);
}

void USPCharacterSelectWidget::OnConfirmClicked()
{
	FString CharacterName = TextBox->GetText().ToString();
	if (CharacterName.Len() <= 0 || CharacterName.Len() > 10) return;

	USPSaveGame* NewPlayerData = NewObject<USPSaveGame>();
	NewPlayerData->PlayerName = CharacterName;
	NewPlayerData->Level = 1;
	NewPlayerData->Exp = 0;
	NewPlayerData->HighScore = 0;
	NewPlayerData->CharacterIndex = CurrentIndex;

	auto SPPlayerState = GetDefault<ASPPlayerState>();
	if (UGameplayStatics::SaveGameToSlot(NewPlayerData, SPPlayerState->SaveSlotName, 0))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Gameplay"));
	}
	else
	{
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}
