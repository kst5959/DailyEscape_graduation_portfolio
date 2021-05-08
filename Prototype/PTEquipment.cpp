// Fill out your copyright notice in the Description page of Project Settings.

#include "PTEquipment.h"
#include "PrototypeCharacter.h"
#include "PTWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "PrototypeNPCCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PTDefenderShield.h"
#include "PTDoctorPassive_Projectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APTEquipment::APTEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	CurrentDefenderShieldAmmo = 1;
	MaxDefenderShieldAmmo = 5;

	CurrentDoctorPassiveAmmo = 10;
	MaxDoctorPassiveAmmo = 40;

	IsFireSuccess = false;

	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APTEquipment::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APTEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APTEquipment::Fire(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	if (CurrentState == EWeaponState::DefenderWeapon)
	{
		
		if (CurrentDefenderShieldAmmo != 0)
		{
			CurrentDefenderShieldAmmo--;
			//서버에서 디펜더 실드 카트리지 스폰
			PlayerCharacter->SetSpawnDefenderShield();
			IsFireSuccess = true;
		}

		else
		{
			IsFireSuccess = false;
			return;
		}

	}

	else if (CurrentState == EWeaponState::DoctorWeapon)
	{
		if (CurrentDoctorPassiveAmmo > 0 )
		{
			CurrentDoctorPassiveAmmo--;
			//서버에서 닥터 패시브 스폰
			PlayerCharacter->SetSpawnDoctorPassiveProjectile();
			IsFireSuccess = true;
		}
		else 
		{
			IsFireSuccess = false;
			return;
		}
	}

}

void APTEquipment::SetActiveSkill(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter)
{
	if (CurrentState == EWeaponState::DefenderWeapon)
	{
		//디펜더 액티브 스킬 서버에서 실행
		PlayerCharacter->SetDefenderActive(SpawnLocation, SpawnRotation, World, this);
		IsFireSuccess = true;
	}

	else if(CurrentState == EWeaponState::DoctorWeapon)
	{
		//닥터 액티브 스킬 서버에서 실행
		PlayerCharacter->SetDoctorActive(SpawnLocation, SpawnRotation, World, this);

		return;
	}

}

void APTEquipment::DefenderActive(FVector SpawnLocation, FRotator SpawnRotation, UWorld* World, APrototypeCharacter* PlayerCharacter)
{
	
}

void APTEquipment::DoctorActive(FVector PlayerLocation, FRotator PlayerRotation, UWorld* World)
{
	
}
