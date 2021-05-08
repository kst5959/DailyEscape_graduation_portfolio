// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPCAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API UNPCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UNPCAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

};
