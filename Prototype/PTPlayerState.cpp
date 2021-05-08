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
* @author : 황현준
* @date : 2021-05-02
* @description : PTPlayerState.cpp에서는 플레이어간 멀티캐스팅을 관리. PlayerState에 구현된 다른 기능들은 블루프린트 BP_PTPlayerState에서 확인.
				 본 소스파일에서는 점프 , 총 발사 몽타주 등 기타 캐릭터 클래스의 멀티캐스팅 구현
				 (게임스테이트와 마찬가지로 추후 리플리케이션 로직을 재 확인해야 할 필요가 있음)
* @updater : 황현준
* @update : 1. 코드 리팩토링 : 변수 명 변경 및 접근자 제한 변경, 점프 시 소모되는 스테미너 값 변수화
*/

APTPlayerState::APTPlayerState()
{
	//디펜더 액티브 스킬 넉백 데미지
	mDefenderActiveDamage = 80.0f;
	//더블 점프 시 소모되는 스테미너 비용
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
* @author : 황현준
* @date : 2021-04-29
* @description : Defender 클래스의 Active Skill : 원형 모양의 범위 안에 있는 모든 적 NPC를 NPC가 보는 방향 반대 방향으로 밀치는 스킬
* @updater : 황현준
* @update : 주석 추가 작업
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

					//서버에서 파티클 처리
					PlayerCharacter->PlayDefenderActiveParticle(NPCCharacter);

					//넉백의 경우 움직일 때랑 정지 상태일 때랑 넉백의 거리가 다르기 때문에, 모두 정지된 상태에서 넉백하도록 함
					if (NPCCharacter->GetVelocity().Y == 0.0f)
					{
						NPCCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 50.0f) * NPCCharacter->GetActorForwardVector() * -1000.0f, false, true);
					}
					else
					{
						NPCCharacter->GetCharacterMovement()->StopActiveMovement();
						NPCCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 50.0f) * NPCCharacter->GetActorForwardVector() * -1000.0f, false, true);
					}

					//서버에서 호출되기 때문에 서버 캐릭터의 중복 실행 방지
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
* @author : 황현준
* @date : 2021-04-29
* @description : Docter 클래스의 Active Skill : 자신 앞의 그로기 상태의 팀원을 다시 소생 시키는 스킬
* @updater : 황현준
* @update : 주석 추가 작업
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
					//플레이어가 그로기 상태일 경우 소생
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