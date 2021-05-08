// Fill out your copyright notice in the Description page of Project Settings.


#include "PTFireArm.h"
#include "PTWeapon.h"
#include "PrototypeProjectile_Soldier.h"
#include "PTSoldierPassive_Projectile.h"
#include "PrototypeProjectile.h"
#include "PrototypeCharacter.h"
#include "PTPlayerController.h"
#include "Engine/World.h"

APTFireArm::APTFireArm()
{
	IsFireSuccess = false;
	
	CurrentFireMode = EFireMode::SemiAuto;
	
	CurrentKnockbackAmmo = 10;
	MaxKnockbackAmmo = 30;

	CurrentSoldierAmmo = 30;
	MaxKSoldierAmmo = 120;

	PrimaryActorTick.bCanEverTick = false;
}


void APTFireArm::BeginPlay()
{
	Super::BeginPlay();
}

void APTFireArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Spawn Projectile KnockabackGun or ClassPassiveWeapon
void APTFireArm::Fire(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Character Name : %s"), *PlayerCharacter->GetName());
	
	if (CurrentState == EWeaponState::KnockbackGun)
	{
		if (CurrentKnockbackAmmo != 0)
		{
			CurrentKnockbackAmmo = CurrentKnockbackAmmo - 1;
			CurrentFireMode = EFireMode::SemiAuto;
			//서버에서 스폰
			PlayerCharacter->SetSpawnKnockbackProjectile();
			
			IsFireSuccess = true;
		}

		else
		{
			IsFireSuccess = false;
			return;
		}

	}
	else if (CurrentState == EWeaponState::SoldierWeapon)
	{
		if (CurrentSoldierAmmo != 0)
		{
			CurrentSoldierAmmo = CurrentSoldierAmmo - 1;
			//연발 사격 가능
			CurrentFireMode = EFireMode::Auto;
			//서버에서 스폰
			PlayerCharacter->SetSpawnSoldierPassiveProjectile();
		
			IsFireSuccess = true;
		}

		else
		{
			IsFireSuccess = false;
			return;
		}
	}
	else
	{
		IsFireSuccess = false;
		return;
	}

}

//Spawn Projectile Class ActiveSkill
void APTFireArm::SetActiveSkill(EWeaponState CurrentState,APrototypeCharacter* PlayerCharacter)
{
	if (PlayerCharacter != nullptr)
	{
		if (CurrentState == EWeaponState::SoldierWeapon)
		{
			if (CurrentSoldierAmmo >= 10)
			{
				CurrentSoldierAmmo = CurrentSoldierAmmo - 10;
				CurrentFireMode = EFireMode::SemiAuto;
				//서버에서 스폰
				PlayerCharacter->SetSpawnSoldierActiveProjectile();

				IsFireSuccess = true;
			}
			else
			{
				IsFireSuccess = false;
				return;
			}
		}
	}
}
