// Fill out your copyright notice in the Description page of Project Settings.


#include "PTDefenderShield.h"
#include "DrawDebugHelpers.h"
#include "PrototypeCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"

// Sets default values
APTDefenderShield::APTDefenderShield()
{
	ChargeParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("ChargeParticle"));
	//지속 시간
	CountTime = 5;
	//캐릭터의 실드 변경 시 실드 실드 장판 안에 있는지 체크
	bUsingDefenderPassiveSkill = false;
	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//실드량
	ShieldValue = 100.0f;
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
}

// Called when the game starts or when spawned
void APTDefenderShield::BeginPlay()
{
	Super::BeginPlay();

	if (bUsingDefenderPassiveSkill == false)
	{
		GetWorldTimerManager().SetTimer(CountTimerHandle, this, &APTDefenderShield::CountTImer, 1.0f, true);
		bUsingDefenderPassiveSkill = true;
	}
	else
		return;
}

// Called every frame
void APTDefenderShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

//실드 장판
void APTDefenderShield::ForceShield()
{
	//UE_LOG(LogTemp, Warning, TEXT("Casting Defender Shield"));
	UWorld* world = GetWorld();
	TArray<FOverlapResult> OverlapResults;

	FCollisionQueryParams CollisionParam(NAME_None, false, this);
	FVector Center = this->GetActorLocation();
	FColor Color = FColor::Yellow;

	bool bResult = world->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel6,
		FCollisionShape::MakeSphere(600.0f),
		CollisionParam
	);

	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			if (OverlapResult.GetActor()->ActorHasTag(TEXT("Player")))
			{
				if (CountTime == 0)
				{
					bUsingDefenderPassiveSkill = false;
				}
				//실드 충전은 서버에서 처리
				APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(OverlapResult.GetActor());
				PlayerCharacter->SetDefenderPassive(PlayerCharacter, ShieldValue, bUsingDefenderPassiveSkill);
		
			}
		}
	}
	DrawDebugSphere(world, Center, 600.0f, 16, Color, false, 0.5f);
}

//지속시간 카운트
void APTDefenderShield::CountTImer()
{
	--CountTime;
	ForceShield();
	if (CountTime < 1)
	{
		GetWorldTimerManager().ClearTimer(CountTimerHandle);
		Destroy();
	}
}

void APTDefenderShield::PlayParticle(AActor* TargetActor)
{
	if (nullptr != ChargeParticle && !(ChargeParticle->IsLooping()))
	{
		if (nullptr == TargetActor)
		{
			return;
		}
		else if (nullptr != TargetActor)
		{
			UGameplayStatics::SpawnEmitterAttached(ChargeParticle, TargetActor->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		}
	}
}

void APTDefenderShield::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APTDefenderShield, CountTime);
	DOREPLIFETIME(APTDefenderShield, bUsingDefenderPassiveSkill);
	DOREPLIFETIME(APTDefenderShield, ShieldValue);

}