// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "AIController.h"
#include "PrototypeAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API APrototypeAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	APrototypeAIController();
	virtual void OnPossess(APawn* InPawn) override;
	
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	
private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;

};
