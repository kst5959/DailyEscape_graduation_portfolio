// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeProjectile_Soldier.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "PTGameStateBase.h"
#include "Particles/ParticleSystem.h"
#include "PrototypeCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"
#include "PrototypeNPCCharacter.h"

// Sets default values
APrototypeProjectile_Soldier::APrototypeProjectile_Soldier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
	RootComponent = CollisionComponent;

	// ProjectileMovementComponent 를 사용하여 이 발사체의 운동을 관장합니다.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->Bounciness = 0.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 1.0f;

	this->Tags.Add(TEXT("SAProjectile"));

	Damage = 50.0f;
	bReplicates = true;

	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CollisionComponent->OnComponentHit.AddDynamic(this, &APrototypeProjectile_Soldier::OnHit);


}

// Called when the game starts or when spawned
void APrototypeProjectile_Soldier::BeginPlay()
{
	Super::BeginPlay();
	if (Character != nullptr)
	{
		CollisionComponent->IgnoreActorWhenMoving(Character, true);
	//	CollisionComponent->IgnoreComponentWhenMoving(Character->GetRootComponent(), true);
		PrimaryActorTick.bCanEverTick = true;
	}

}

// Called every frame
void APrototypeProjectile_Soldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APrototypeProjectile_Soldier::SetIgnoreCollision(APrototypeCharacter* PlayerCharacter)
{
	if (this != nullptr)
	{
		if (PlayerCharacter != nullptr)
		{
			CollisionComponent->IgnoreActorWhenMoving(PlayerCharacter, true);
			CollisionComponent->IgnoreComponentWhenMoving(PlayerCharacter->GetCapsuleComponent(), true);
		}
	}
}


void APrototypeProjectile_Soldier::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
		UWorld* world = GetWorld();
		TArray<FOverlapResult> OverlapResults;

		FCollisionQueryParams CollisionParam(NAME_None, false, this);
		FVector Center = this->GetActorLocation();
		FColor Color = FColor::Orange;
		CollisionParam.AddIgnoredActor(this);

		bool bResult = world->OverlapMultiByChannel(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECollisionChannel::ECC_EngineTraceChannel5,
			FCollisionShape::MakeSphere(600.0f),
			CollisionParam
		);

		APTGameStateBase* PTGameState = Cast<APTGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
		if (bResult)
		{
			for (FOverlapResult& OverlapResult : OverlapResults)
			{
				if (OverlapResult.GetActor()->ActorHasTag(TEXT("NPC")))
				{
					APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());

					//발사체 같은 경우 게임 스테이트에서 이펙트 처리
					if (PTGameState != nullptr)
					{
						PTGameState->PlaySoldierActiveHitParticleOnServer(this, OverlapResult.GetActor(),HitParticle, GetActorLocation());
					}

					//멀티 캐스트 시 서버 플레이어는 2번 실행되기에 중복 방지
					if (GetLocalRole() == ROLE_Authority)
					{
						UGameplayStatics::ApplyDamage(Pawn, Damage, Pawn->GetController(), Character, UDamageType::StaticClass());
					}

					Color = FColor::Green;
				}
			}
		}

		//발사체 같은 경우 게임 스테이트에서 이펙트 처리
		if (PTGameState != nullptr)
		{
			PTGameState->PlaySoldierActiveExplosionParticleOnServer(this,ExplosionParticle,ExplosionSound, GetActorLocation());
			DrawDebugSphere(world, Center, 600.0f, 16, Color, false, 0.5f);
		}

		Destroy();
}

//폭발 파티클
void APrototypeProjectile_Soldier::PlayExplosionParticle()
{
	if (this != nullptr)
	{
		if (ExplosionParticle != nullptr && ExplosionSound != nullptr)
		{
			if (this != nullptr)
			{
				UWorld* world = GetWorld();
			
				UGameplayStatics::SpawnEmitterAtLocation(world, ExplosionParticle, this->GetActorLocation());
				UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetRootComponent()->GetComponentLocation());

			}
		}
	}
}

//피격 파티클
void APrototypeProjectile_Soldier::PlayHitParticle(AActor* TargetActor)
{
	if (this != nullptr)
	{
		if (nullptr != HitParticle)
		{
			if (nullptr == TargetActor)
			{
				return;
			}
			else if (nullptr != TargetActor)
			{
				UGameplayStatics::SpawnEmitterAttached(HitParticle, TargetActor->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
			}
		}
	}
}

void APrototypeProjectile_Soldier::Acceleration()
{
	ProjectileMovementComponent->MaxSpeed = 6000.0f;
	ProjectileMovementComponent->InitialSpeed = 6000.0f;
	Damage = 60.0f;
}