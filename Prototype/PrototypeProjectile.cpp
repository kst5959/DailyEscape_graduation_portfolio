// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PrototypeProjectile.h"
#include "PrototypeNPCCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PrototypeCharacter.h"
#include "Engine.h"
#include "Components/SphereComponent.h"

APrototypeProjectile::APrototypeProjectile() 
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
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	this->Tags.Add(TEXT("KBProjectile"));
	Damage = 25.0f;

	bReplicates = true;

	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CollisionComp->OnComponentHit.AddDynamic(this, &APrototypeProjectile::OnHit);		// set up a notification for when this component hits something blocking


}

void APrototypeProjectile::SetIgnoreCollision(APrototypeCharacter* PlayerCharacter)
{
	if (this != nullptr)
	{
		if (PlayerCharacter != nullptr)
		{
			CollisionComp->IgnoreActorWhenMoving(PlayerCharacter, true);
		}
	}
}

void APrototypeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if (GetLocalRole() == ROLE_Authority)
	{
		if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
		{
			if (OtherActor->ActorHasTag(TEXT("Player")) || OtherActor->ActorHasTag(TEXT("NPC")))
			{
				ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);

				if (nullptr != HitCharacter)
				{
					if (HitCharacter->GetVelocity().Y == 0.0f)
					{
						if (HitCharacter->bWasJumping)
						{
							HitCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 1.0f) * (GetProjectileMovement()->Velocity * 0.5f), false, true);
						}
						else
						{
							HitCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 1.0f) * (GetProjectileMovement()->Velocity * 0.5f), false, false);
						}
					}
					else
					{
						HitCharacter->GetCharacterMovement()->StopActiveMovement();

						if (HitCharacter->bWasJumping)
						{
							HitCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 1.0f) * (GetProjectileMovement()->Velocity * 0.5f), false, true);
						}
						else
						{
							HitCharacter->LaunchCharacter(FVector(10.0f, 10.0f, 1.0f) * (GetProjectileMovement()->Velocity * 0.5f), false, false);
						}
					}
			
					if (OtherActor->ActorHasTag(TEXT("NPC")))
					{
						APrototypeNPCCharacter* NPCCharacter = Cast<APrototypeNPCCharacter>(OtherActor);
						UGameplayStatics::ApplyDamage(NPCCharacter, Damage, NPCCharacter->GetController(), Character, UDamageType::StaticClass());
					}
				}
			}

			OtherComp->AddImpulse(FVector(10.0f, 10.0f, 1.0f) *(GetProjectileMovement()->Velocity * 0.1f), NAME_None, true);
			HitComp->AddImpulse(FVector(10.0f, 10.0f, 1.0f)* (GetProjectileMovement()->Velocity * 0.1f), NAME_None, true);
		
			Destroy();
		}
	}
}

void APrototypeProjectile::Acceleration()
{
	ProjectileMovement->MaxSpeed = 6000.0f;
	ProjectileMovement->InitialSpeed = 6000.0f;
	Damage = 30.0f;
}