// Fill out your copyright notice in the Description page of Project Settings.


#include "PTGameStateBase.h"
#include "PrototypeProjectile_Soldier.h"
#include "PrototypeCharacter.h"
#include "Engine/World.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PrototypeNPCCharacter.h"

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : ���ӽ�����Ʈ �ҽ����Ͽ����� �߻�ü�� ��ƼŬ ��Ƽĳ������ ����. ���ӽ�����Ʈ�� ������ �ٸ� ��ɵ��� �������Ʈ BP_PTGameStateBase���� Ȯ��.
				 ��ƼŬ ��Ƽĳ���� ��δ� ������ ����
				 PrototypeProjectile_Soldier �� �߻�ü Ŭ�������� �ǰ� ���� �� ��ƼŬ ��� ���� �˻� -> PTGameStateBase.cpp�� �߻�ü ���� ������ ����
				 -> ������ ���� ������ �״�� ��Ƽĳ��Ʈ �Լ��� ���� -> �߻�ü Ŭ�������� ��ƼŬ ���
				 (��ƼŬ ���ø����̼� ���� ������ ���� �� Ȯ�� �ؾ� �� �ʿ䰡 ����)
* @updater : Ȳ����
* @update : �ڵ� �����丵 : ������ �������� �Լ� �� ����
*/

APTGameStateBase::APTGameStateBase()
{
	
}

void APTGameStateBase::PlaySoldierActiveExplosionParticleOnServer_Implementation(class APrototypeProjectile_Soldier* SoldierActiveProjectile, UParticleSystem* Particle, USoundBase* Sound, FVector SpawnLocation)
{
	PlaySoldierActiveExplosionParticleMulticast(SoldierActiveProjectile, Particle, Sound, SpawnLocation);
}

void APTGameStateBase::PlaySoldierActiveHitParticleOnServer_Implementation(class APrototypeProjectile_Soldier* SoldierActiveProjectile, AActor* TargetActor, UParticleSystem* Particle, FVector SpawnLocation)
{
	PlaySoldierActiveHitParticleMulticast(SoldierActiveProjectile, TargetActor, Particle, SpawnLocation);
}

void APTGameStateBase::PlaySoldierActiveExplosionParticleMulticast_Implementation(class APrototypeProjectile_Soldier* SoldierActiveProjectile, UParticleSystem* Particle, USoundBase* Sound, FVector SpawnLocation)
{
	if (SoldierActiveProjectile != nullptr)
	{
		if (Particle != nullptr)
		{
			SoldierActiveProjectile->PlayExplosionParticle();
		}
	}
}

void APTGameStateBase::PlaySoldierActiveHitParticleMulticast_Implementation(class APrototypeProjectile_Soldier* SoldierActiveProjectile, AActor* TargetActor, UParticleSystem* Particle, FVector SpawnLocation)
{
	if (SoldierActiveProjectile != nullptr)
	{
		if (Particle != nullptr)
		{
			SoldierActiveProjectile->PlayHitParticle(TargetActor);
		}
	}
}

void APTGameStateBase::SetDoctorPassiveSlow_Implementation(APrototypeNPCCharacter* TargetActor)
{
	if (TargetActor != nullptr)
	{
		TargetActor->Slow();
	}
}

void APTGameStateBase::SetDocterPassiveHeal_Implementation(APrototypeCharacter* TargetActor)
{
	if (TargetActor != nullptr)
	{
		TargetActor->HP = TargetActor->HP + TargetActor->DoctorHeal;
		if (TargetActor->ActorHasTag(TEXT("Defender")))
		{
			if (TargetActor->HP > 150.0f)
			{
				TargetActor->HP = 150.0f;
			}
		}
		else
		{
			if (TargetActor->HP > 100.0f)
			{
				TargetActor->HP = 100.0f;
			}
		}
	}
}
