// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PTPlayerController.generated.h"

UCLASS()
class PROTOTYPE_API APTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	APTPlayerController();

public:

	/** �÷��̾� ��� �� ������ ��� -> BP_PTPlayerController���� ȣ�� */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category ="Event")
	void ChangeSpectatorMode();

	/** NPC ü�� ���� */
	UFUNCTION(Server, Reliable)
	void SetNPCHP(AActor* TargetActor , float Damage);

	/** ���� �� �Ҹ�Ǵ� ���׹̳� ���ø����̼� */
	UFUNCTION()
	void ReplicateJumpStamina(APrototypeCharacter* PlayerCharacter, bool bPressedJumping);

	/** ��ų ��� �� �Ҹ�Ǵ� ��ų��ٿ� ���ø����̼� */
	UFUNCTION()
	void ReplicateActiveCooldown(APrototypeCharacter* PlayerCharacter);

	/** ĳ���� �̵� �ӵ� ���ø����̼� */
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateCharacterWalkSpeed(APrototypeCharacter* PlayerCharacter, float Speed, bool bPressedRun);

	/** ĳ���� Surf ��� �� ���ø����̼� */
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateSurf(APrototypeCharacter* PlayerCharacter, float Speed, bool bPressedSurf);

	UFUNCTION(Server, Reliable)
	void PlayFireMontage(APrototypeCharacter* PlayerCharacter);
	
	UFUNCTION(Server, Reliable)
	void PlayReloadMontage(UPTAnimInstance* BodyMeshAnimInstance , APrototypeCharacter* PlayerCharacter);

	UFUNCTION(Server, Reliable)
	void PlaySoundAndParticle(APrototypeCharacter* PlayerCharacter, UParticleSystem* Particle, USoundBase* Sound);

	UFUNCTION(Server, Reliable)
	void TurretHacking(APTTurret* Turret);

	UFUNCTION(Server,Reliable)
	void TurretFire(APTTurret* Turret , AActor* TargetActor , UWorld* World);

};
