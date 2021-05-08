// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMInimal.h"
#include "GameFramework/Character.h"
#include "PrototypeNPCCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class PROTOTYPE_API APrototypeNPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeNPCCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION()
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void Slow();

	UFUNCTION(BlueprintCallable)
	bool GetIsHit();

	UFUNCTION(BlueprintCallable)
	void SetIsHit(bool bIsHit);

	UFUNCTION(BlueprintCallable)
	bool GetIsAttaking();

	UFUNCTION(BlueprintCallable)
	void SetIsAttaking(bool bIsAttacking);

	UFUNCTION(BlueprintCallable)
	void SetHP(float Damage);

	UFUNCTION(BlueprintCallable)
	void AttackCheck();

	UFUNCTION()
	void PlayDefenderActiveParticle();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	UMaterialInterface* SlowMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	UMaterialInterface* CharacterMaterial;

	UPROPERTY(Replicated)
	class APTPlayerController* PTAIController;

	UPROPERTY(BlueprintReadWrite, Replicated)
	class APrototypeCharacter* DamageCauserPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle,Replicated)
	class UParticleSystem* DefenderActiveParticle;

	FOnAttackEndDelegate OnAttackEnd;

	FTimerHandle TimerHandler;

	FORCEINLINE class APrototypeCharacter* GetDamageCauserPlayer() const { return DamageCauserPlayer; }

protected:

	UPROPERTY()
	APawn* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float mSlowCountTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bIsSlow;

	UPROPERTY(BlueprintReadWrite, Category = Stat, Replicated)
	float mHP;

	UPROPERTY(BlueprintReadWrite, Category = Stat)
	float mFinalDamage;

	UPROPERTY(EditAnywhere, Replicated)
	bool bIsAttacking;

	UPROPERTY(Replicated)
	bool bIsHit;

};
