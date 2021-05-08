// Fill out your copyright notice in the Description page of Project Settings.


#include "PTGameStateBase.h"
#include "PrototypeProjectile_Soldier.h"
#include "PrototypeCharacter.h"
#include "Engine/World.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PrototypeNPCCharacter.h"

/**
* @author : 황현준
* @date : 2021-05-02
* @description : 게임스테이트 소스파일에서는 발사체의 파티클 멀티캐스팅을 관리. 게임스테이트에 구현된 다른 기능들은 블루프린트 BP_PTGameStateBase에서 확인.
				 파티클 멀티캐스팅 경로는 다음과 같다
				 PrototypeProjectile_Soldier 등 발사체 클래스에서 피격 판정 및 파티클 재생 여부 검사 -> PTGameStateBase.cpp로 발사체 정보 서버로 전달
				 -> 서버로 받은 정보를 그대로 멀티캐스트 함수로 전달 -> 발사체 클래스에서 파티클 재생
				 (파티클 리플리케이션 과정 로직은 추후 재 확인 해야 할 필요가 있음)
* @updater : 황현준
* @update : 코드 리팩토링 : 가독성 개선으로 함수 명 변경
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
