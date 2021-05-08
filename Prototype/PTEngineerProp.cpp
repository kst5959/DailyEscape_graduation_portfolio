// Fill out your copyright notice in the Description page of Project Settings.


#include "PTEngineerProp.h"
#include "PrototypeCharacter.h"
#include "PTTurret.h"
#include "PTWeapon.h"
#include "PTAcceleration.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
APTEngineerProp::APTEngineerProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	IsFireSuccess = false;
	//�ͷ� ���� ����
	IsTurretAccessed = false;
	//��ŷ ���� ����
	IsHackingCompleted = false;
	//��ŷ ��Ʈ ���� ����
	HintNumber.Init(0, 9);
	CurrentTurret = nullptr;
	//��Ʈ ���� ��Ʈ�� ��ȯ ó��
	HintNumberToString = NULL;
	//�Է��� ����
	Answer = NULL;
	
}


// Called when the game starts or when spawned
void APTEngineerProp::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APTEngineerProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

//�нú� ��ų - ��ŷ
void APTEngineerProp::SetPassiveSkill(FVector StartPoint, FVector GetForwardVector, UWorld* World , EWeaponState CurrentState)
{
	FHitResult HitResult;

	FCollisionQueryParams CollisionParam(NAME_None, false, this);
	FVector End = StartPoint + GetForwardVector * 200.0f;

	DrawDebugLine(World, StartPoint, End, FColor::Blue, true, 2.0f , 2.0f); // ����׶��� ����

	bool isHit = World->LineTraceSingleByChannel(HitResult, StartPoint, End, ECC_Visibility, CollisionParam);

	if (isHit)
	{
		if (HitResult.GetActor()->ActorHasTag(TEXT("Turret")))
		{
			APTTurret* Turret = Cast<APTTurret>(HitResult.GetActor());
			//������ �ͷ��� ���°� �߸��� ���
			if (Turret->GetTurretState() == ETurretState::Neutral)
			{
				//�ͷ� ���� �Ϸ�
				IsTurretAccessed = true;
				CurrentTurret = HitResult.GetActor();

				//���� ��Ʈ ����
				for (int32 i = 0; i < 9; i++)
				{
					HintNumber[i] = FMath::RandRange(1, HintNumber.Num() - 1);
					HintNumberToString = HintNumberToString + FString::FromInt(HintNumber[i]);
					UE_LOG(LogTemp, Warning, TEXT("index %d"), HintNumber[i]);
				}

				UE_LOG(LogTemp, Warning, TEXT("str =  %s "), *HintNumberToString);
				//�ͷ��� ��ŷ ���� �޾ƿ��� (UI���� ó�� �� ��)
				IsHackingCompleted = Turret->GetIsHackingCompleted();
			}
			else
				return;
			
		}
	}
}

//��Ƽ�� ��ų - ���� ����
void APTEngineerProp::SetActiveSkill(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter)
{
	if (CurrentState == EWeaponState::EngineerWeapon)
	{
		//�������� ���������̼� ����
		PlayerCharacter->SetSpawnEngineerActive();
		IsFireSuccess = true;
		
	}
	else
	{
		IsFireSuccess = false;
		return;
	}

}

