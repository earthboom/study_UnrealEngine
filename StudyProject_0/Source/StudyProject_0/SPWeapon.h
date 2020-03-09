// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "GameFramework/Actor.h"
#include "SPWeapon.generated.h"

UCLASS()
class STUDYPROJECT_0_API ASPWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
};
