// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "Components/ActorComponent.h"
#include "SPCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelgate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYPROJECT_0_API USPCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USPCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;	

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float newDamage);
	float GetAttack();

	FOnHPIsZeroDelgate OnHPIsZero;

private:
	struct FSPCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;
};
