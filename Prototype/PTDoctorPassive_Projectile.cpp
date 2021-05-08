// Fill out your copyright notice in the Description page of Project Settings.

#include "PTDoctorPassive_Projectile.h"
#include "Components/SphereComponent.h"
#include "PrototypeNPCCharacter.h"
#include "PTGameStateBase.h"
#include "PrototypeCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
APTDoctorPassive_Projectile::APTDoctorPassive_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComponent->InitSphereRadius(2.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentHit.AddDynamic(this, &APTDoctorPassive_Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComponent;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = 5000.f;
	ProjectileMovementComponent->MaxSpeed = 5000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void APTDoctorPassive_Projectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APTDoctorPassive_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APTDoctorPassive_Projectile::SetIgnoreCollision(APrototypeCharacter* PlayerCharacter)
{
	if (this != nullptr)
	{
		if (PlayerCharacter != nullptr)
		{
			CollisionComponent->IgnoreActorWhenMoving(PlayerCharacter, true);
		}
	}
}

void APTDoctorPassive_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr)
	{
		//게임 스테이트에서 몬스터 슬로우
		APTGameStateBase* PTGameState = Cast<APTGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));

		if (OtherActor->ActorHasTag(TEXT("NPC")))
		{
			//서버에서 슬로우 처리
			APrototypeNPCCharacter* NPCCharacter = Cast<APrototypeNPCCharacter>(OtherActor);
			PTGameState->SetDoctorPassiveSlow(NPCCharacter);
		}
		else if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			//멀티 캐스트 중복 방지
			if (GetLocalRole() == ROLE_Authority)
			{
				//서버에서 힐 처리
				APrototypeCharacter* OtherCharacter = Cast<APrototypeCharacter>(OtherActor);
				PTGameState->SetDocterPassiveHeal(OtherCharacter);
				
			}
		}
	}
	Destroy();
	
}
