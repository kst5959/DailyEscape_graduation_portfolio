// Fill out your copyright notice in the Description page of Project Settings.


#include "PTPlayerState.h"
#include "PrototypeCharacter.h"
#include "PTWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "Particles/ParticleSystem.h"
#include "PrototypeNPCCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PTDefenderShield.h"
#include "PTDoctorPassive_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "PTAnimInstance.h"

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : PTPlayerState.cpp������ �÷��̾ ��Ƽĳ������ ����. PlayerState�� ������ �ٸ� ��ɵ��� �������Ʈ BP_PTPlayerState���� Ȯ��.
				 �� �ҽ����Ͽ����� ���� , �� �߻� ��Ÿ�� �� ��Ÿ ĳ���� Ŭ������ ��Ƽĳ���� ����
				 (���ӽ�����Ʈ�� ���������� ���� ���ø����̼� ������ �� Ȯ���ؾ� �� �ʿ䰡 ����)
* @updater : Ȳ����
* @update : 1. �ڵ� �����丵 : ���� �� ���� �� ������ ���� ����, ���� �� �Ҹ�Ǵ� ���׹̳� �� ����ȭ
*/

APTPlayerState::APTPlayerState()
{
	//����� ��Ƽ�� ��ų �˹� ������
	mDefenderActiveDamage = 80.0f;
	//���� ���� �� �Ҹ�Ǵ� ���׹̳� ���
	mJumpStaminaCost = 20.0f;
}

void APTPlayerState::SetJumpStaminaOnServer_Implementation(APrototypeCharacter* PlayerCharacter, bool bPressedJumping)
{
	if (PlayerCharacter != nullptr)
	{
		SetJumpStaminaMulticast(PlayerCharacter, bPressedJumping);
	}
}

void APTPlayerState::SetJumpStaminaMulticast_Implementation(APrototypeCharacter* PlayerCharacter, bool bPressedJumping)
{
	if (PlayerCharacter != nullptr)
	{
		if (bPressedJumping)
		{
			PlayerCharacter->Stamina = PlayerCharacter->Stamina - mJumpStaminaCost;
		}
	}
}

void APTPlayerState::SetActiveCooldownOnServer_Implementation(APrototypeCharacter* PlayerCharacter)
{
	if (PlayerCharacter != nullptr)
	{
		SetActiveCooldownMulticast(PlayerCharacter);
	}
}

void APTPlayerState::SetActiveCooldownMulticast_Implementation(APrototypeCharacter* PlayerCharacter)
{
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->ActiveSkillCooldown = 0.0f;
	}
}


/**
* @author : Ȳ����
* @date : 2021-04-29
* @description : Defender Ŭ������ Active Skill : ���� ����� ���� �ȿ� �ִ� ��� �� NPC�� NPC�� ���� ���� �ݴ� �������� ��ġ�� ��ų
* @updater : Ȳ����
* @update : �ּ� �߰� �۾�
* @param : 
*/
void APTPlayerState::DefenderActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter)
{
	//UE_LOG(LogTemp, Warning, TEXT("Defender Active"));
	TArray<FOverlapResult> OverlapResults;

	FCollisionQueryParams CollisionParam(NAME_None, false, this);
	FVector Center = SpawnLocation;
	FColor Color = FColor::Cyan;

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel4,
		FCollisionShape::MakeSphere(600.0f),
		CollisionParam
	);

	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			if (OverlapResult.GetActor() != nullptr)
			{
				if (OverlapResult.GetActor()->ActorHasTag(TEXT("NPC")))
				{
					APrototypeNPCCharacter * NPCCharacter = Cast<APrototypeNPCCharacter>(OverlapResult.GetActor());

					//�������� ��ƼŬ ó��
					PlayerCharacter->PlayDefenderActiveParticle(NPCCharacter);

					//�˹��� ��� ������ ���� ���� ������ ���� �˹��� �Ÿ��� �ٸ��� ������, ��� ������ ���¿��� �˹��ϵ��� ��
					if (NPCCharacter->GetVelocity().Y == 0.0f)
					{
						NPCCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 50.0f) * NPCCharacter->GetActorForwardVector() * -1000.0f, false, true);
					}
					else
					{
						NPCCharacter->GetCharacterMovement()->StopActiveMovement();
						NPCCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 50.0f) * NPCCharacter->GetActorForwardVector() * -1000.0f, false, true);
					}

					//�������� ȣ��Ǳ� ������ ���� ĳ������ �ߺ� ���� ����
					if (HasAuthority())
					{
						UGameplayStatics::ApplyDamage(NPCCharacter, mDefenderActiveDamage, NPCCharacter->GetController(), PlayerCharacter, UDamageType::StaticClass());
					}
				}
			}
		}
	}
	DrawDebugSphere(World, Center, 600.0f, 16, Color, false, 0.3f);
}


/**
* @author : Ȳ����
* @date : 2021-04-29
* @description : Docter Ŭ������ Active Skill : �ڽ� ���� �׷α� ������ ������ �ٽ� �һ� ��Ű�� ��ų
* @updater : Ȳ����
* @update : �ּ� �߰� �۾�
*/
void APTPlayerState::DoctorActive(FVector PlayerLocation, FRotator PlayerRotation, UWorld* World)
{
	TArray<FOverlapResult> OverlapResults;

	FCollisionQueryParams Params(NAME_None, false, this);

	FCollisionQueryParams CollisionParam(NAME_None, false, this);
	FVector PlayerCenter = PlayerLocation;
	FColor Color = FColor::Blue;

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		PlayerCenter,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel4,
		FCollisionShape::MakeSphere(600.0f),
		CollisionParam
	);

	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			if (OverlapResult.GetActor() != nullptr)
			{
				if (OverlapResult.GetActor()->ActorHasTag(TEXT("Player")))
				{
					//�÷��̾ �׷α� ������ ��� �һ�
					APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(OverlapResult.GetActor());
					if (PlayerCharacter->GetPlayerState() == EPlayerState::Groggy)
					{
						PlayerCharacter->SetRevive();
						break;
					}
				}
			}
		}
	}

	FVector TraceVec = PlayerLocation;
	FVector Center = PlayerLocation + TraceVec * 0.5f;
	float HalfHeight = 50.0f * 0.5f + 50.0f;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(World,
		PlayerLocation,
		HalfHeight,
		50.0f,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

}

void APTPlayerState::PlayFireMontage_Implementation(APrototypeCharacter* PTCharacter)
{
	if (PTCharacter != nullptr)
	{
		PTCharacter->BodyMeshAnimInstance->TPPPlayFIreMontage();
	}
}

void APTPlayerState::PlayReloadMontage_Implementation(APrototypeCharacter* PTCharacter)
{
	if (PTCharacter != nullptr)
	{
		PTCharacter->BodyMeshAnimInstance->TPPPlayReloadMontage();
	}
}

void APTPlayerState::PlaySoundAndParticle_Implementation(APrototypeCharacter* PlayerCharacter, UParticleSystem* Particle, USoundBase* Sound)
{
	if (PlayerCharacter != nullptr)
	{
		if (Particle != nullptr)
		{
			if (PlayerCharacter->CurrentWeaponState == EWeaponState::KnockbackGun || PlayerCharacter->CurrentWeaponState == EWeaponState::SoldierWeapon)
			{
				if (!PlayerCharacter->IsLocallyControlled())
				{
					UGameplayStatics::PlaySoundAtLocation(PlayerCharacter, Sound, PlayerCharacter->GetActorLocation());
				}

				UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(Particle, PlayerCharacter->GetThirdPersonMuzzleLocation(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
				ParticleSystemComponent->bOwnerNoSee = true;
			}
		}
	}
}

void APTPlayerState::SetAcceleration_Implementation(APrototypeCharacter* PlayerCharacter)
{
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->Acceleration();
	}
}

void APTPlayerState::SetDefenderPassive_Implementation(APrototypeCharacter* PlayerCharacter, float ShieldValue, bool bUsingDefenderPassive)
{
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->SetArmor(ShieldValue, bUsingDefenderPassive);
	}
}