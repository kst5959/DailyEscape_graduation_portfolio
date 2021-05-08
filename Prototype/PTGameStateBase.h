// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PTGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API APTGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	APTGameStateBase();

	UFUNCTION(Server, Reliable)
	void PlaySoldierActiveExplosionParticleOnServer(class APrototypeProjectile_Soldier* SoldierActiveProjectile, UParticleSystem* Particle, USoundBase* Sound, FVector SpawnLocation);

	UFUNCTION(Server, Reliable)
	void PlaySoldierActiveHitParticleOnServer(class APrototypeProjectile_Soldier* SoldierActiveProjectile, AActor* TargetActor, UParticleSystem* Particle, FVector SpawnLocation);

	UFUNCTION(NetMulticast, Reliable)
	void PlaySoldierActiveExplosionParticleMulticast(class APrototypeProjectile_Soldier* SoldierActiveProjectile, UParticleSystem* Particle, USoundBase* Sound, FVector SpawnLocation);

	UFUNCTION(NetMulticast, Reliable)
	void PlaySoldierActiveHitParticleMulticast(class APrototypeProjectile_Soldier* SoldierActiveProjectile, AActor* TargetActor, UParticleSystem* Particle, FVector SpawnLocation);

	UFUNCTION(NetMulticast, Reliable)
	void SetDoctorPassiveSlow(APrototypeNPCCharacter* TargetActor);

	UFUNCTION(Server, Reliable)
	void SetDocterPassiveHeal(APrototypeCharacter* TargetActor);

};
