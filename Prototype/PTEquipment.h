// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTEquipment.generated.h"

UCLASS()
class PROTOTYPE_API APTEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Fire(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	void SetActiveSkill(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	void DefenderActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	void DoctorActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int CurrentDefenderShieldAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int MaxDefenderShieldAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int CurrentDoctorPassiveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int MaxDoctorPassiveAmmo;


	UPROPERTY()
	bool IsFireSuccess;

	FDamageEvent DamageEvent;

};
