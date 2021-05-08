// Fill out your copyright notice in the Description page of Project Settings.


#include "PTWeapon.h"
#include "PrototypeCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/NameTypes.h"
#include "Net/UnrealNetwork.h"
#include "PTFireArm.h"

// Sets default values
APTWeapon::APTWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentWeaponState = EWeaponState::KnockbackGun;

}

// Called when the game starts or when spawned
void APTWeapon::BeginPlay()
{
	Super::BeginPlay();

	KnockbackGunMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("SkeletalMesh'/Game/FirstPerson/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	SoldierWeaponMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("SkeletalMesh'/Game/TPS_Multiplayer_Pack/Blueprints/Weapons/WeaponsMesh/Rifle.Rifle'"));

	DoctorStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Meshes/DoctorWeapon/DoctorWeapon.DoctorWeapon'"));
	DefenderStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Meshes/DefenderWeapon/DefenderWeapon.DefenderWeapon'"));
	EngineerStaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Meshes/EngineerWeapon/EngineerWeapon.EngineerWeapon'"));

}

// Called every frame
void APTWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//웨폰 메쉬의 경우 애셋 부족으로 스켈레탈 메쉬와 스태틱 메쉬 2가지 경우로 있어서 다 같이 처리 해줘야 함
//스켈레탈 메쉬 : 넉백건 / 솔저건
//스태틱 메쉬 : 닥터 건 / 디펜더 실드 / 엔지니어 태블릿 

//1인칭 무기 메쉬 넉백건으로 변경
void APTWeapon::ChangeKnockbackGun(class APrototypeCharacter* Character)
{
	if ((CurrentWeaponState != EWeaponState::KnockbackGun))
	{
		CurrentWeaponState = EWeaponState::KnockbackGun;
		Character->GetFPGun()->SetSkeletalMesh(KnockbackGunMesh);
		//클래스 웨폰이 스태틱 메쉬인 경우 메쉬 제거
		Character->GetStaticWeaponMesh()->SetStaticMesh(nullptr);

		//3인칭 메쉬도 변경
		ThirdPersonChangeKnockbackGun(Character);
	}
	
	
}

//1인칭 무기 메쉬 클래스 웨폰으로 변경
void APTWeapon::ChangeClassWeapon(class APrototypeCharacter* Character)
{
	if (Character->ActorHasTag(TEXT("Soldier")))
	{
		if (GetWeaponState() == EWeaponState::KnockbackGun)
		{
			CurrentWeaponState = EWeaponState::SoldierWeapon;
			Character->GetFPGun()->SetSkeletalMesh(SoldierWeaponMesh);
			Character->GetStaticWeaponMesh()->SetRelativeLocationAndRotation(FVector(2.0f, 22.0f, 10.0f), FRotator(0.0f, 90.0f, 0.0f));
			Character->GetMuzzleLocation()->SetRelativeLocationAndRotation(FVector(100.0f, 35.0f, 135.0f), FRotator(0.0f, 0.0f, 0.0f));

		}
		ThirdPersonChangeClassWeapon(Character);
	}

	else if (Character->ActorHasTag(TEXT("Defender")))
	{
		if (GetWeaponState() == EWeaponState::KnockbackGun)
		{
			CurrentWeaponState = EWeaponState::DefenderWeapon;
			Character->GetFPGun()->SetSkeletalMesh(nullptr);
			Character->GetStaticWeaponMesh()->SetStaticMesh(DefenderStaticMesh);
			Character->GetStaticWeaponMesh()->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
			Character->GetStaticWeaponMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 5.0f), FRotator(0.0f, 90.0f, -90.0f));
		}
		ThirdPersonChangeClassWeapon(Character);
	}

	else if (Character->ActorHasTag(TEXT("Doctor")))
	{
		if (GetWeaponState() == EWeaponState::KnockbackGun)
		{
			CurrentWeaponState = EWeaponState::DoctorWeapon;
			Character->GetFPGun()->SetSkeletalMesh(nullptr);
			Character->GetStaticWeaponMesh()->SetStaticMesh(DoctorStaticMesh);
			Character->GetStaticWeaponMesh()->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
			Character->GetStaticWeaponMesh()->SetRelativeLocationAndRotation(FVector(2.0f, 16.0f, 10.0f), FRotator(-10.0f, 0.0f, -15.0f));
			Character->GetMuzzleLocation()->SetRelativeLocationAndRotation(FVector(100.0f, 35.0f, 135.0f), FRotator(0.0f, 0.0f, 0.0f));
		}
		ThirdPersonChangeClassWeapon(Character);
	}

	else if (Character->ActorHasTag(TEXT("Engineer")))
	{
		if (GetWeaponState() == EWeaponState::KnockbackGun)
		{
			CurrentWeaponState = EWeaponState::EngineerWeapon;
			Character->GetFPGun()->SetSkeletalMesh(nullptr);
			Character->GetStaticWeaponMesh()->SetStaticMesh(EngineerStaticMesh);
			Character->GetStaticWeaponMesh()->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
		}
		ThirdPersonChangeClassWeapon(Character);
	}
	else
		return;
	
}

void APTWeapon::ThirdPersonChangeKnockbackGun(class APrototypeCharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("tpp knockback"));

	if (GetWeaponState() == EWeaponState::KnockbackGun)
	{
		Character->GetThirdPersonSkeletalMeshWeapon()->SetSkeletalMesh(KnockbackGunMesh);
		Character->GetThirdPersonSkeletalMeshWeapon()->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
		Character->GetThirdPersonStaticMeshWeapon()->SetStaticMesh(nullptr);
	}
}

void APTWeapon::ThirdPersonChangeClassWeapon(class APrototypeCharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("tpp class weapon"));

	if (Character->ActorHasTag(TEXT("Soldier")))
	{
		if (GetWeaponState() == EWeaponState::SoldierWeapon)
		{
			Character->GetThirdPersonSkeletalMeshWeapon()->SetSkeletalMesh(SoldierWeaponMesh);	
		}
	}

	else if (Character->ActorHasTag(TEXT("Defender")))
	{
		if (GetWeaponState() == EWeaponState::DefenderWeapon)
		{
			Character->GetThirdPersonSkeletalMeshWeapon()->SetSkeletalMesh(nullptr);
			Character->GetThirdPersonStaticMeshWeapon()->SetStaticMesh(DefenderStaticMesh);
			Character->GetThirdPersonStaticMeshWeapon()->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
			Character->GetThirdPersonStaticMeshWeapon()->SetRelativeLocationAndRotation(FVector(0.0f, 1.0f, 5.0f), FRotator(0.0f, 99.0f, -90.0f));
		}
	}

	else if (Character->ActorHasTag(TEXT("Doctor")))
	{
		if (GetWeaponState() == EWeaponState::DoctorWeapon)
		{
			Character->GetThirdPersonSkeletalMeshWeapon()->SetSkeletalMesh(nullptr);
			Character->GetThirdPersonStaticMeshWeapon()->SetStaticMesh(DoctorStaticMesh);
			Character->GetThirdPersonStaticMeshWeapon()->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
			Character->GetThirdPersonStaticMeshWeapon()->SetRelativeLocationAndRotation(FVector(2.0f, 26.0f, 8.0f), FRotator(0.0f, 0.0f, 0.0f));
		}
	}

	else if (Character->ActorHasTag(TEXT("Engineer")))
	{
		if (GetWeaponState() == EWeaponState::EngineerWeapon)
		{
			Character->GetThirdPersonSkeletalMeshWeapon()->SetSkeletalMesh(nullptr);
			Character->GetThirdPersonStaticMeshWeapon()->SetStaticMesh(EngineerStaticMesh);
			Character->GetThirdPersonStaticMeshWeapon()->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
		}
	}
}

void APTWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APTWeapon, KnockbackGunMesh);
	DOREPLIFETIME(APTWeapon, SoldierWeaponMesh);
	DOREPLIFETIME(APTWeapon, DefenderStaticMesh);
	DOREPLIFETIME(APTWeapon, EngineerStaticMesh);
	DOREPLIFETIME(APTWeapon, DoctorStaticMesh);

	DOREPLIFETIME(APTWeapon, CurrentWeaponState);

}
