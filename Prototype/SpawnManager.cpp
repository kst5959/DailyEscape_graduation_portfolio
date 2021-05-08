// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "Components/BoxComponent.h"
#include "PrototypeNPCCharacter.h"
#include "Net/UnrealNetwork.h"
#include "BTService_Detect.h"

/**  
* @author : 황현준
* @date : 2021-05-02
* @description : 적 NPC 동적 생성을 위한 트리거 CPP
* @updater : 황현준 
* @update : 코드 리팩토링 : 변수 명 / 및 접근자 제한 변경
*/

ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	mSetSpawnVar = 5;
	mSetCollisionBoxSize = FVector(5.0f, 5.0f, 5.0f);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionComponent->SetBoxExtent(mSetCollisionBoxSize, false);
	CollisionComponent->SetCollisionProfileName("Trigger");
}

void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	BP_PrototypeNPCCharacter = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/Actors/BP_PrototypeNPCCharacter.BP_PrototypeNPCCharacter_C"));
}

/**
* @author : 황현준
* @date : 2021-04-29
* @description : BP_SpawnTriggerBox1에 플레이어 진입 시 SpawnManager 태그 식별 후 함수 호출로 NPC 스폰
* @updater : 황현준
* @update : 코드 가독성 및 nullptr 검사로 코드 안정성 개선
* @param : 
*/
void ASpawnManager::SpawnAlien()
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	UWorld* world = GetWorld();
	APrototypeNPCCharacter* Alien = world->SpawnActor<APrototypeNPCCharacter>(BP_PrototypeNPCCharacter, GetActorLocation(), GetActorRotation(), ActorSpawnParams);

	if (Alien != nullptr)
	{
		Alien->Tags.Add(TEXT("DynamicSpawnActor"));
	}
		
	mSetSpawnVar--;

	if (mSetSpawnVar == 0)
	{
		return;
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASpawnManager::SpawnAlien, 3.0f, false);
	}
}
