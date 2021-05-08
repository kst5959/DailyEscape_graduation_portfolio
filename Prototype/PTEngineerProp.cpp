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
	//터렛 접속 여부
	IsTurretAccessed = false;
	//해킹 성공 여부
	IsHackingCompleted = false;
	//해킹 힌트 숫자 생성
	HintNumber.Init(0, 9);
	CurrentTurret = nullptr;
	//힌트 숫자 스트링 변환 처리
	HintNumberToString = NULL;
	//입력한 정답
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

//패시브 스킬 - 해킹
void APTEngineerProp::SetPassiveSkill(FVector StartPoint, FVector GetForwardVector, UWorld* World , EWeaponState CurrentState)
{
	FHitResult HitResult;

	FCollisionQueryParams CollisionParam(NAME_None, false, this);
	FVector End = StartPoint + GetForwardVector * 200.0f;

	DrawDebugLine(World, StartPoint, End, FColor::Blue, true, 2.0f , 2.0f); // 디버그라인 생성

	bool isHit = World->LineTraceSingleByChannel(HitResult, StartPoint, End, ECC_Visibility, CollisionParam);

	if (isHit)
	{
		if (HitResult.GetActor()->ActorHasTag(TEXT("Turret")))
		{
			APTTurret* Turret = Cast<APTTurret>(HitResult.GetActor());
			//접속한 터렛의 상태가 중립일 경우
			if (Turret->GetTurretState() == ETurretState::Neutral)
			{
				//터렛 접속 완료
				IsTurretAccessed = true;
				CurrentTurret = HitResult.GetActor();

				//랜덤 힌트 생성
				for (int32 i = 0; i < 9; i++)
				{
					HintNumber[i] = FMath::RandRange(1, HintNumber.Num() - 1);
					HintNumberToString = HintNumberToString + FString::FromInt(HintNumber[i]);
					UE_LOG(LogTemp, Warning, TEXT("index %d"), HintNumber[i]);
				}

				UE_LOG(LogTemp, Warning, TEXT("str =  %s "), *HintNumberToString);
				//터렛의 해킹 상태 받아오기 (UI에서 처리 한 뒤)
				IsHackingCompleted = Turret->GetIsHackingCompleted();
			}
			else
				return;
			
		}
	}
}

//액티브 스킬 - 가속 관문
void APTEngineerProp::SetActiveSkill(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter)
{
	if (CurrentState == EWeaponState::EngineerWeapon)
	{
		//서버에서 엑셀러레이션 생성
		PlayerCharacter->SetSpawnEngineerActive();
		IsFireSuccess = true;
		
	}
	else
	{
		IsFireSuccess = false;
		return;
	}

}

