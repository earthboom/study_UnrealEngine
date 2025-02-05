// Fill out your copyright notice in the Description page of Project Settings.


#include "SPCharacterStatComponent.h"
#include "SPGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
USPCharacterStatComponent::USPCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	Level = 1;
	// ...
}


// Called when the game starts
void USPCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USPCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void USPCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto SPGameInstance = Cast<USPGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != SPGameInstance);
	CurrentStatData = SPGameInstance->GetSPCharacterData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHp);
		CurrentHP = CurrentStatData->MaxHp;
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void USPCharacterStatComponent::SetDamage(float newDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	SetHP(FMath::Clamp<float>(CurrentHP - newDamage, 0.0f, CurrentStatData->MaxHp));
}

void USPCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

float USPCharacterStatComponent::GetAttack() const
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}

float USPCharacterStatComponent::GetHPRatio() const
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);

	return (CurrentStatData->MaxHp < KINDA_SMALL_NUMBER) ? 0.0 : (CurrentHP / CurrentStatData->MaxHp);
}

int32 USPCharacterStatComponent::GetDropExp() const
{
	return CurrentStatData->DropExp;
}
