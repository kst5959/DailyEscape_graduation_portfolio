// Fill out your copyright notice in the Description page of Project Settings.


#include "PTSoldierPassive_Projectile.h"
#include "PTGameStateBase.h"
#include "PTPlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PrototypeNPCCharacter.h"
#include "PrototypeCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine.h"
#include "Components/SphereComponent.h"

APTSoldierPassive_Projectile::APTSoldierPassive_Projectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	this->Tags.Add(TEXT("SPProjectile"));
	Damage = 40.0f;

	bReplicates = true;
	
	CollisionComp->OnComponentHit.AddDynamic(this, &APTSoldierPassive_Projectile::OnHit);		// set up a notification for when this component hits something blocking
	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	PrimaryActorTick.bCanEverTick = false;
}

void APTSoldierPassive_Projectile::SetIgnoreCollision(APrototypeCharacter* PlayerCharacter)
{
	if (this != nullptr)
	{
		if (PlayerCharacter != nullptr)
		{
			CollisionComp->IgnoreActorWhenMoving(PlayerCharacter, true);
		}
	}
}

void APTSoldierPassive_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
		{
			APrototypeNPCCharacter* NPCCharacter = Cast<APrototypeNPCCharacter>(OtherActor);

			if (nullptr != NPCCharacter)
			{
				if (NPCCharacter->ActorHasTag(TEXT("NPC")))
				{
	
					if (Character != nullptr)
					{
						UGameplayStatics::ApplyDamage(NPCCharacter, Damage, NPCCharacter->GetController(), Character, UDamageType::StaticClass());
					}
					
				}
			}
		}

		Destroy();
	}
}


void APTSoldierPassive_Projectile::Acceleration()
{
	ProjectileMovement->MaxSpeed = 10000.0f;
	ProjectileMovement->InitialSpeed = 10000.0f;
	Damage = 50.0f;
}
