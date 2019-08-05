// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseActor.generated.h"

UCLASS()
class TWINSTICKSHOOTER_API ABaseActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BluePrintReadWrite, EditAnywhere, Category = "Base Actor")
		float LaserGage = 0.0f;

	UPROPERTY(BluePrintReadWrite, EditAnywhere, Category = "Base Actor")
		bool bMouseClick = false;

	UFUNCTION(BlueprintCallable, Category = "Base Actor")
		virtual void LaserGageReset();

	UFUNCTION(BlueprintCallable, Category = "Base Actor")
		virtual void onGage(float& _f);

public:	
	// Sets default values for this actor's properties
	ABaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
