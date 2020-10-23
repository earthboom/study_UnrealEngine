// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StudyProject_0.h"
#include "AIController.h"
#include "SPAIController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_0_API ASPAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASPAIController();
	virtual void OnPossess(APawn* InPawn) override;
	//virtual void OnUnPossess() override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
	//void OnRepeatTimer();

	//FTimerHandle RepeatTimerHandle;
	//float RepeatInterval;
};
