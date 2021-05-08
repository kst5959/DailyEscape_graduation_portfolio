// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PTPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API APTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	APTPlayerState();
	
	UFUNCTION(Server, Reliable)
	void SetJumpStaminaOnServer(APrototypeCharacter* PlayerCharacter, bool bPressedJumping);

	UFUNCTION(NetMulticast, Reliable)
	void SetJumpStaminaMulticast(APrototypeCharacter* PlayerCharacter, bool bPressedJumping);

	UFUNCTION(Server, Reliable)
	void SetActiveCooldownOnServer(APrototypeCharacter* PlayerCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void SetActiveCooldownMulticast(APrototypeCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	void DefenderActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	void DoctorActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void PlayFireMontage(APrototypeCharacter* Character);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void PlayReloadMontage(APrototypeCharacter* Character);

	UFUNCTION(NetMulticast, Reliable,BlueprintCallable)
	void PlaySoundAndParticle(APrototypeCharacter* PlayerCharacter, UParticleSystem* Particle, USoundBase* Sound);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void SetAcceleration(APrototypeCharacter* PlayerCharacter);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void SetDefenderPassive(APrototypeCharacter* PlayerCharacter, float ShieldVar, bool bUsingDefenderPassive);

private:

	UPROPERTY()
	float mDefenderActiveDamage;

	UPROPERTY()
	float mJumpStaminaCost;
};
