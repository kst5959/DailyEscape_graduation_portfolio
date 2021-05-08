// Fill out your copyright notice in the Description page of Project Settings.

#include "PTAcceleration.h"
#include "PrototypeCharacter.h"
#include "PrototypeProjectile.h"
#include "PrototypeProjectile_Soldier.h"
#include "PTSoldierPassive_Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

// Sets default values
APTAcceleration::APTAcceleration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName("Trigger");
	
	RootComponent = BoxComponent;

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APTAcceleration::OnOverlapBegin);

	Duration = 5.0f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void APTAcceleration::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APTAcceleration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Duration != 0.0f)
	{
		Duration = Duration - (1.0f * DeltaTime);
	}
	if (Duration <= 0)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
}

void APTAcceleration::OnOverlapBegin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		Acceleration(OtherActor);
		
	}
	else
		return;
}

//���� ���� ���� �� ���� ȿ�� -> ������Ÿ���� ���� �� ������ ���� ȿ���� ���� ���ø����̼� �� �� (�ش� ��� ����)
void APTAcceleration::Acceleration(AActor* Object)
{
	if (Object != nullptr)
	{
		if (Object->ActorHasTag(TEXT("Player")))
		{
			//������ �����ؼ� ���� �� �̵� �ӵ� ����ȭ
			APrototypeCharacter* Character = Cast<APrototypeCharacter>(Object);
			Character->SetAcceleration();
			
		}
		else if (Object->ActorHasTag(TEXT("KBProjectile")))
		{
			UE_LOG(LogTemp, Warning, TEXT("KBProjectile"));
			APrototypeProjectile* Projectile = Cast<APrototypeProjectile>(Object);
			Projectile->Acceleration();
		}
		else if (Object->ActorHasTag(TEXT("SAProjectile")))
		{
			UE_LOG(LogTemp, Warning, TEXT("SAProjectile"));
			APrototypeProjectile_Soldier* Projectile = Cast<APrototypeProjectile_Soldier>(Object);
			Projectile->Acceleration();
		}
		else if (Object->ActorHasTag(TEXT("SPProjectile")))
		{
			UE_LOG(LogTemp, Warning, TEXT("SPProjectile"));
			APTSoldierPassive_Projectile* Projectile = Cast<APTSoldierPassive_Projectile>(Object);
			Projectile->Acceleration();
		}

	}
}