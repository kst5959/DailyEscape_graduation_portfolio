// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTSoldierPassive_Projectile.generated.h"

UCLASS()
class PROTOTYPE_API APTSoldierPassive_Projectile : public AActor
{
	GENERATED_BODY()
	
	/** Sphere collision component */


public:
	APTSoldierPassive_Projectile();

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Damage;

	UFUNCTION()
	void Acceleration();

	UPROPERTY()
	APawn* Character;

	UFUNCTION(BlueprintCallable)
	void SetIgnoreCollision(APrototypeCharacter* PlayerCharacter);


private:

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
};
