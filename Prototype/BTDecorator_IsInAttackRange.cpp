// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "PrototypeAIController.h"
#include "PrototypeNPCCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PrototypeCharacter.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (nullptr ==ControllingPawn)
	{
		return false;
	}

	auto Target = Cast<APrototypeCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(APrototypeAIController::TargetKey));
	if (nullptr == Target)
	{
		return false;
	}

	bResult = (Target->GetDistanceTo(ControllingPawn) <= 100.0f);
	//UE_LOG(LogTemp, Warning, TEXT("IsInAttackRange"));
	return bResult;

}
