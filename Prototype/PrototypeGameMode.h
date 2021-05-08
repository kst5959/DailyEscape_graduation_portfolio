// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrototypeGameMode.generated.h"

UCLASS(minimalapi)
class APrototypeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APrototypeGameMode();

	UFUNCTION()
	void SpawnSoldierActiveProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter);

	UFUNCTION()
	void SpawnSoldierPassiveProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter);

	UFUNCTION()
	void SpawnKnockbackProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter);
	
	UFUNCTION()
	void SpawnDefenderPassive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World);

	UFUNCTION()
	void SpawnDoctorPassiveProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter);

	UFUNCTION()
	void SpawnEngineerActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World);


private:

	UPROPERTY(Category = "FireArm", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_SoldierActiveProjectlieClass;

	UPROPERTY(Category = "FireArm", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_KnockbackProjectlieClass;

	UPROPERTY(Category = "FireArm", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_SoldierPassiveProjectlieClass;

	UPROPERTY(Category = "Equipment", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_DefenderPassiveClass;

	UPROPERTY(Category = "Equipment", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_DoctorPassiveProjectileClass;

	UPROPERTY(Category = "EngineerProp", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_EngineerActiveClass;

};



