// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTDefenderShield.generated.h"

UCLASS()
class PROTOTYPE_API APTDefenderShield : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTDefenderShield();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ForceShield();

	void CountTImer();

	FTimerHandle CountTimerHandle;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 CountTime;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float ShieldValue;

	UPROPERTY()
	APawn* Character;

	UFUNCTION()
	void PlayParticle(AActor* Target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = Particle)
	UParticleSystem* ChargeParticle;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bUsingDefenderPassiveSkill;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
