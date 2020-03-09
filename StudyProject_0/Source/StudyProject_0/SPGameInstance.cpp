// Fill out your copyright notice in the Description page of Project Settings.


#include "SPGameInstance.h"
#include "UObject/ConstructorHelpers.h"

USPGameInstance::USPGameInstance()
{
	FString CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_SPCHARACTER(*CharacterDataPath);
	ABCHECK(DT_SPCHARACTER.Succeeded());
	SPCharacterTable = DT_SPCHARACTER.Object;
	ABCHECK(SPCharacterTable->GetRowMap().Num() > 0);
}

void USPGameInstance::Init()
{
	Super::Init();
	//ABLOG(Warning, TEXT("DropExp of Level 20 SPCharacter : %d"), GetSPCharacterData(20)->DropExp);
}

FSPCharacterData* USPGameInstance::GetSPCharacterData(int32 Level)
{
	return SPCharacterTable->FindRow<FSPCharacterData>(*FString::FromInt(Level), TEXT(""));
}
