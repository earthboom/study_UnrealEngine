// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "SPGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSPCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSPCharacterData() 
		:Level(1), MaxHp(100.0f), Attack(10.0f), DropExp(10), NextExp(10){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 NextExp;
};

UCLASS()
class STUDYPROJECT_0_API USPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USPGameInstance();

	virtual void Init() override;
	FSPCharacterData* GetSPCharacterData(int32 Level);

	FStreamableManager StreamableManager;

private:
	UPROPERTY()
	class UDataTable* SPCharacterTable;
};
