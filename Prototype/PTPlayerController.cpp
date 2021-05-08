// Fill out your copyright notice in the Description page of Project Settings.


#include "PTPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "PrototypeCharacter.h"
#include "PTGameStateBase.h"
#include "PTAnimInstance.h"
#include "PTEngineerProp.h"
#include "PTTurret.h"
#include "PTWeapon.h"
#include "PTPlayerState.h"
#include "PTEquipment.h"
#include "PTDefenderShield.h"
#include "PTAcceleration.h"
#include "PTSoldierPassive_Projectile.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Kismet/KismetMathLibrary.h"
#include "PrototypeNPCCharacter.h"

/**
* @author : 황현준
* @date : 2021-05-02
* @description : PTPlayerController.cpp 에서는 캐릭터들 간 애니메이션 및 상호작용 멀티태스킹 관리. PlayerController에 구현된 다른 기능들은 블루프린트 BP_PTPlayerController에서 확인.
				 현재 PTPlayerController.cpp에서 관리하는 기능은 다음과 같다
			     엔지니어 클래스의 Turret Hacking, 플레이어 캐릭터의 몽타주, 이동속도, 스테미너, 점프나 서프 같은 무브먼트 기능 리플리케이션
				 적 NPC HP 관리
* @updater : 황현준
* @update : 1. 코드 리팩토링 : 가독성 개선으로 함수 명 변경
*/

APTPlayerController::APTPlayerController()
{

}

void APTPlayerController::SetNPCHP_Implementation(AActor* TargetActor, float Damage)
{
	if (ROLE_Authority ==GetLocalRole())
	{
		if (TargetActor != nullptr)
		{
			if (TargetActor->ActorHasTag(TEXT("NPC")))
			{
				APrototypeNPCCharacter* NPC = Cast<APrototypeNPCCharacter>(TargetActor);
				APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(GetPawn());
				NPC->SetIsHit(true);
				NPC->SetHP(Damage);
				NPC->DamageCauserPlayer = PlayerCharacter;
			}
		}
	}
}

void APTPlayerController::ReplicateJumpStamina(APrototypeCharacter* PlayerCharacter, bool bPressedJumping)
{
	if (PlayerCharacter != nullptr)
	{
		APTPlayerState* PTPlayerState = Cast<APTPlayerState>(PlayerState);
		if (PTPlayerState != nullptr)
		{
			if (HasAuthority())
			{
				if (GetLocalRole() ==ROLE_Authority)
				{
					PTPlayerState->SetJumpStaminaMulticast(PlayerCharacter, bPressedJumping);
				}
			}
			else if(!HasAuthority())
			{
				PTPlayerState->SetJumpStaminaOnServer(PlayerCharacter, bPressedJumping);
			}
		}
	}
}

void APTPlayerController::ReplicateActiveCooldown(APrototypeCharacter* PlayerCharacter)
{
	if (PlayerCharacter != nullptr)
	{
		APTPlayerState* PTPlayerState = Cast<APTPlayerState>(PlayerState);
		if (PTPlayerState != nullptr)
		{
			if (HasAuthority())
			{
				if (GetLocalRole() == ROLE_Authority)
				{
					PTPlayerState->SetActiveCooldownMulticast(PlayerCharacter);
				}
			}
			else if (!HasAuthority())
			{
				PTPlayerState->SetActiveCooldownOnServer(PlayerCharacter);
			}
		}

	}
}

void APTPlayerController::ReplicateCharacterWalkSpeed_Implementation(APrototypeCharacter* PlayerCharacter, float Speed ,bool bPressedRun)
{
	if (PlayerCharacter != nullptr)
	{

		if (bPressedRun == true)
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
			PlayerCharacter->bPressedRun = true;
			//	UE_LOG(LogTemp, Warning, TEXT("Running %f "), Speed);
		}
		else if (bPressedRun == false)
		{

			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
			PlayerCharacter->bPressedRun = false;
			//	UE_LOG(LogTemp, Warning, TEXT("Stop Running %f "), Speed);
		}

	}

}

void APTPlayerController::ReplicateSurf_Implementation(APrototypeCharacter* PlayerCharacter, float Speed, bool bPressedSurf)
{
	if (PlayerCharacter != nullptr)
	{
		if (bPressedSurf == true)
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
			PlayerCharacter->GetCharacterMovement()->SetWalkableFloorAngle(90.0f);
			PlayerCharacter->bPrssedSurf = true;
			//	UE_LOG(LogTemp, Warning, TEXT("Surf %f "), Speed);
		}
		else if (bPressedSurf == false)
		{
			if (PlayerCharacter->bPressedRun == false)
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
			}
			PlayerCharacter->GetCharacterMovement()->SetWalkableFloorAngle(45.0f);
			PlayerCharacter->bPrssedSurf = false;
			//	UE_LOG(LogTemp, Warning, TEXT("Stop Surf %f "), Speed);
		}

	}

}

/**
* @author : 황현준
* @date : 2021-04-29
* @description : 플레이어의 발사체 발사 몽타주 및 총구 파티클 리플리케이션은 다음과 같이 구현했다.
				 서버에서 -> 다른 클라이언트로 애니메이션 및 파티클 전달 : PlayerState에서 NetMulticast
				 클라이언트 -> 다른 클라이언트로 애니메이션 및 파티클 전달 : PlayerState에서 NetMulticast
				 클라이언트 -> 서버로 애니메이션 및 파티클 전달 : PlayerController에서 Server
* @updater : 황현준
* @update : 주석 업데이트
* @param : PrototypeCharacter
*/
void APTPlayerController::PlayFireMontage_Implementation(APrototypeCharacter* PlayerCharacter)
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(PlayerState);

	if (PlayerCharacter != nullptr)
	{
		if (HasAuthority())
		{
			PTPlayerState->PlayFireMontage(PlayerCharacter);
		}

		else if (!HasAuthority())
		{
			PlayerCharacter->BodyMeshAnimInstance->TPPPlayFIreMontage();
			PTPlayerState->PlayFireMontage(PlayerCharacter);
		}
	}

}

void APTPlayerController::PlayReloadMontage_Implementation(UPTAnimInstance* BodyMeshAnimInstance, APrototypeCharacter* PlayerCharacter)
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(PlayerState);

	if (PlayerCharacter != nullptr)
	{
		if (HasAuthority())
		{
			PTPlayerState->PlayReloadMontage(PlayerCharacter);
		}

		else if (!HasAuthority())
		{
			PlayerCharacter->BodyMeshAnimInstance->TPPPlayReloadMontage();
			PTPlayerState->PlayReloadMontage(PlayerCharacter);
		}
	}
}

void APTPlayerController::PlaySoundAndParticle_Implementation(APrototypeCharacter* PlayerCharacter, UParticleSystem* Particle, USoundBase* Sound)
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(PlayerState);
	if (PTPlayerState != nullptr)
	{
		if (PlayerCharacter != nullptr)
		{
			PTPlayerState->PlaySoundAndParticle(PlayerCharacter, Particle, Sound);
		}
	}
}

void APTPlayerController::TurretHacking_Implementation(APTTurret* Turret)
{
	if (Turret != nullptr)
	{
		Turret->CurrentTurretState = ETurretState::Activated;
	}
}

void APTPlayerController::TurretFire_Implementation(APTTurret* Turret, AActor* TargetActor, UWorld* World)
{
	if (Turret != nullptr)
	{
		if (TargetActor != nullptr)
		{
			Turret->Fire();
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}