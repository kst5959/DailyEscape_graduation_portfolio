// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PrototypeGameMode.h"
#include "PrototypeHUD.h"
#include "PTPlayerController.h"
#include "PTPlayerState.h"
#include "PrototypeCharacter.h"
#include "PrototypeProjectile.h"
#include "PrototypeProjectile_Soldier.h"
#include "PTSoldierPassive_Projectile.h"
#include "PrototypeNPCCharacter.h"
#include "PTDefenderShield.h"
#include "PTAcceleration.h"
#include "PTDoctorPassive_Projectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : PrototypeGameMode.cpp ������ �߻�ü ���� ����. GameMode�� ������ �ٸ� ��ɵ��� �������Ʈ BP_PrototypeGameMode���� Ȯ��.
				 �߻�ü ���� ��δ� ������ ����.
				 PrototypeCharacter.cpp ���� �߻� ��� -> PTFireArm.cpp �� �߻�ü ���� Ŭ�������� �߻� ���� ���� �˻� 
				 -> PrototypeCharacter.cpp �߻�ü ���� ���� ���� -> PrototypeGameMode.cpp ���� �߻�ü ����
* @updater : Ȳ����
* @update : �ڵ� �����丵 : ������ ������ �Լ� �� ���� �� ����
*/

APrototypeGameMode::APrototypeGameMode()
	: Super()
{
	PlayerControllerClass = APTPlayerController::StaticClass();

	HUDClass = APrototypeHUD::StaticClass();
	
	BP_KnockbackProjectlieClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonProjectile.FirstPersonProjectile_C"));
	BP_SoldierActiveProjectlieClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/BP_PrototypeProjectile_Soldier.BP_PrototypeProjectile_Soldier_C"));
	BP_SoldierPassiveProjectlieClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/BP_PTSoldierPassive_Projectile.BP_PTSoldierPassive_Projectile_C"));
	BP_DefenderPassiveClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/PTDefenderShield.PTDefenderShield_C"));
	BP_DoctorPassiveProjectileClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/BP_PTDoctorPassive_Projectile.BP_PTDoctorPassive_Projectile_C"));

	BP_EngineerActiveClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/BP_PTAcceleration.BP_PTAcceleration_C"));

}


void APrototypeGameMode::SpawnSoldierActiveProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (PlayerCharacter != nullptr)
	{
		if (BP_SoldierActiveProjectlieClass != nullptr)
		{
			APrototypeProjectile_Soldier* Projectile = World->SpawnActor<APrototypeProjectile_Soldier>(BP_SoldierActiveProjectlieClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (Projectile != nullptr)
			{
				Projectile->SetIgnoreCollision(PlayerCharacter);
			}
		}
	}

}

void APrototypeGameMode::SpawnSoldierPassiveProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (PlayerCharacter != nullptr)
	{
		if (BP_SoldierPassiveProjectlieClass != nullptr)
		{
			APTSoldierPassive_Projectile* Projectile = World->SpawnActor<APTSoldierPassive_Projectile>(BP_SoldierPassiveProjectlieClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (Projectile != nullptr)
			{
				Projectile->SetIgnoreCollision(PlayerCharacter);
			}
		}
	}
}

void APrototypeGameMode::SpawnKnockbackProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (PlayerCharacter != nullptr)
	{
		APrototypeProjectile* Projectile = World->SpawnActor<APrototypeProjectile>(BP_KnockbackProjectlieClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Projectile != nullptr)
		{
			Projectile->Character = PlayerCharacter;
			Projectile->SetIgnoreCollision(PlayerCharacter);
		}
	}
}

void APrototypeGameMode::SpawnDefenderPassive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (BP_DefenderPassiveClass != nullptr)
	{
		APTDefenderShield* Shield = World->SpawnActor<APTDefenderShield>(BP_DefenderPassiveClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}
}

void APrototypeGameMode::SpawnDoctorPassiveProjectile(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (PlayerCharacter != nullptr)
	{
		if (BP_DoctorPassiveProjectileClass != nullptr)
		{
			APTDoctorPassive_Projectile* Projectile = World->SpawnActor<APTDoctorPassive_Projectile>(BP_DoctorPassiveProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			if (Projectile != nullptr)
			{
				Projectile->SetIgnoreCollision(PlayerCharacter);
			}
		}
	}
}

void APrototypeGameMode::SpawnEngineerActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (BP_EngineerActiveClass != nullptr)
	{
		APTAcceleration* Aceeleration = World->SpawnActor<APTAcceleration>(BP_EngineerActiveClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}

}

