// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "SPAIController.h"
#include "SPCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	if (nullptr == World) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults, Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);
	
	if (bResult)
	{
		//ABLOG(Warning, TEXT("OverlapResults size : %d"), OverlapResults.Num());
		//OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASPAIController::TargetKey, nullptr);
		for(auto const& OverlapResult : OverlapResults)		
		{
			ASPCharacter* SPCharacter = Cast<ASPCharacter>(OverlapResult.GetActor());
			
			ABLOG(Warning, TEXT("SPCharacter->GetActorLocation() : %f, %f, %f"), 
				SPCharacter->GetActorLocation().X, SPCharacter->GetActorLocation().Y, SPCharacter->GetActorLocation().Z);

			if (SPCharacter && SPCharacter->GetController()->IsPlayerController())
			{
				ABLOG(Warning, TEXT("TEST 1111"));
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASPAIController::TargetKey, SPCharacter);
				
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, SPCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), SPCharacter->GetActorLocation(), FColor::Blue, false, 0.27f);
				
				return;
			}
		}
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASPAIController::TargetKey, nullptr);
	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}
