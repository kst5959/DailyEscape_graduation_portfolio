// Fill out your copyright notice in the Description page of Project Settings.


#include "PTAnimInstance.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "PTPlayerController.h"
#include "PrototypeCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : PTAnimInstance.cpp������ ��Ÿ�ָ� �����ϰ� �ٸ� �ִϸ��̼��� �������Ʈ���� ����
* @updater : Ȳ����
* @update :
*/
UPTAnimInstance::UPTAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> TPPFire_Montage(TEXT("/Game/Meshes/Animations/TPCAnimation/Fire_Rifle_Ironsights_Montage.Fire_Rifle_Ironsights_Montage"));
	if (TPPFire_Montage.Succeeded())
	{
		TPPFireMontage = TPPFire_Montage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> TPPReload_Montage(TEXT("/Game/TPS_Multiplayer_Pack/Characters/Animations/Anim_Montages/Reload_Rifle_Hip_Montage.Reload_Rifle_Hip_Montage"));
	if (TPPReload_Montage.Succeeded())
	{
		TPPReloadMontage = TPPReload_Montage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FPPReload_Montage(TEXT("/Game/FirstPerson/Animations/FPP_Reload_Montage.FPP_Reload_Montage"));
	if (FPPReload_Montage.Succeeded())
	{
		ReloadMontage = FPPReload_Montage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FPPFire_Montage(TEXT("/Game/FirstPerson/Animations/FirstPersonFire_Montage.FirstPersonFire_Montage"));
	if (FPPFire_Montage.Succeeded())
	{
		FireMontage = FPPFire_Montage.Object;
	}

	
}

//3��Ī �� �߻� ��Ÿ��
void UPTAnimInstance::TPPPlayFIreMontage_Implementation()
{
	if (!(Montage_IsPlaying(TPPFireMontage)))
	{
		Montage_Play(TPPFireMontage, 0.25f);
	}

}

//3��Ī �� ������ ��Ÿ��
void UPTAnimInstance::TPPPlayReloadMontage()
{
	if (!(Montage_IsPlaying(TPPReloadMontage)))
	{
		Montage_Play(TPPReloadMontage, 2.5f);
	}
}

//1��Ī �� ������ ��Ÿ��
void UPTAnimInstance::PlayReloadMontage()
{
	if (!(Montage_IsPlaying(ReloadMontage)))
	{
		Montage_Play(ReloadMontage, 2.5f);
	}
}

//1��Ī �� �߻� ��Ÿ��
void UPTAnimInstance::PlayFIreMontage()
{
	if (!(Montage_IsPlaying(FireMontage)))
	{
		Montage_Play(FireMontage, 1.0f);
	}
}
