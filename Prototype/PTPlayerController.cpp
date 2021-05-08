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
* @author : Ȳ����
* @date : 2021-05-02
* @description : PTPlayerController.cpp ������ ĳ���͵� �� �ִϸ��̼� �� ��ȣ�ۿ� ��Ƽ�½�ŷ ����. PlayerController�� ������ �ٸ� ��ɵ��� �������Ʈ BP_PTPlayerController���� Ȯ��.
				 ���� PTPlayerController.cpp���� �����ϴ� ����� ������ ����
			     �����Ͼ� Ŭ������ Turret Hacking, �÷��̾� ĳ������ ��Ÿ��, �̵��ӵ�, ���׹̳�, ������ ���� ���� �����Ʈ ��� ���ø����̼�
				 �� NPC HP ����
* @updater : Ȳ����
* @update : 1. �ڵ� �����丵 : ������ �������� �Լ� �� ����
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
* @author : Ȳ����
* @date : 2021-04-29
* @description : �÷��̾��� �߻�ü �߻� ��Ÿ�� �� �ѱ� ��ƼŬ ���ø����̼��� ������ ���� �����ߴ�.
				 �������� -> �ٸ� Ŭ���̾�Ʈ�� �ִϸ��̼� �� ��ƼŬ ���� : PlayerState���� NetMulticast
				 Ŭ���̾�Ʈ -> �ٸ� Ŭ���̾�Ʈ�� �ִϸ��̼� �� ��ƼŬ ���� : PlayerState���� NetMulticast
				 Ŭ���̾�Ʈ -> ������ �ִϸ��̼� �� ��ƼŬ ���� : PlayerController���� Server
* @updater : Ȳ����
* @update : �ּ� ������Ʈ
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