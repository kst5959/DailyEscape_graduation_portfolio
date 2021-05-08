// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeAIController.h"
#include "PrototypeNPCCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Net/UnrealNetwork.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName APrototypeAIController::HomePosKey(TEXT("HomePos"));
const FName APrototypeAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName APrototypeAIController::TargetKey(TEXT("Target"));

APrototypeAIController::APrototypeAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/AI/BB_PTCharacter.BB_PTCharacter'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/AI/BT_PTCharacter.BT_PTCharacter'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
	
}

void APrototypeAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIController couldnt run behaviortree"));
		}
	}

}
