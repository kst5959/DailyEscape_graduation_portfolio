// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();

	UPROPERTY()
	float DetectRadius; 

	UPROPERTY()
	bool IsFindTarget;

	UFUNCTION()
	void SetDetectRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
