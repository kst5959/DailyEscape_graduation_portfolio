// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "Components/BoxComponent.h"
#include "PrototypeNPCCharacter.h"
#include "Net/UnrealNetwork.h"
#include "BTService_Detect.h"

/**  
* @author : Ȳ����
* @date : 2021-05-02
* @description : �� NPC ���� ������ ���� Ʈ���� CPP
* @updater : Ȳ���� 
* @update : �ڵ� �����丵 : ���� �� / �� ������ ���� ����
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
* @author : Ȳ����
* @date : 2021-04-29
* @description : BP_SpawnTriggerBox1�� �÷��̾� ���� �� SpawnManager �±� �ĺ� �� �Լ� ȣ��� NPC ����
* @updater : Ȳ����
* @update : �ڵ� ������ �� nullptr �˻�� �ڵ� ������ ����
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
