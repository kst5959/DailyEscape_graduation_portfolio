// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PrototypeProjectile_Soldier.generated.h"

UCLASS()
class PROTOTYPE_API APrototypeProjectile_Soldier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APrototypeProjectile_Soldier();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	USphereComponent* CollisionComponent;

	UPROPERTY(BlueprintReadWrite)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComponent; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = Particle)
	class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
	class UParticleSystem* ExplosionParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
	class USoundBase* ExplosionSound;

	UFUNCTION(BlueprintCallable)
	void PlayHitParticle(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void PlayExplosionParticle();

	UFUNCTION(BlueprintCallable)
	void SetIgnoreCollision(APrototypeCharacter* PlayerCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Damage;

	UFUNCTION()
	void Acceleration();

	UPROPERTY()
	APawn* Character;


};
