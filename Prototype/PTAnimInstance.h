// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "PTAnimInstance.generated.h"


UCLASS()
class PROTOTYPE_API UPTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPTAnimInstance();

	UFUNCTION(NetMulticast, Reliable)
	void TPPPlayFIreMontage();

	UFUNCTION(BlueprintCallable)
	void TPPPlayReloadMontage();

	UFUNCTION(BlueprintCallable)
	void PlayReloadMontage();

	UFUNCTION(BlueprintCallable)
	void PlayFIreMontage();


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Fire)
	UAnimMontage* TPPFireMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Reload)
	UAnimMontage* TPPReloadMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Reload)
	UAnimMontage* ReloadMontage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Reload)
	UAnimMontage* FireMontage;

};
