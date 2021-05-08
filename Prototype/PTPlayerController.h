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

	/** 플레이어 사망 시 관전자 모드 -> BP_PTPlayerController에서 호출 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category ="Event")
	void ChangeSpectatorMode();

	/** NPC 체력 변경 */
	UFUNCTION(Server, Reliable)
	void SetNPCHP(AActor* TargetActor , float Damage);

	/** 점프 시 소모되는 스테미너 리플리케이션 */
	UFUNCTION()
	void ReplicateJumpStamina(APrototypeCharacter* PlayerCharacter, bool bPressedJumping);

	/** 스킬 사용 시 소모되는 스킬쿨다운 리플리케이션 */
	UFUNCTION()
	void ReplicateActiveCooldown(APrototypeCharacter* PlayerCharacter);

	/** 캐릭터 이동 속도 리플리케이션 */
	UFUNCTION(NetMulticast, Reliable)
	void ReplicateCharacterWalkSpeed(APrototypeCharacter* PlayerCharacter, float Speed, bool bPressedRun);

	/** 캐릭터 Surf 사용 시 리플리케이션 */
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
