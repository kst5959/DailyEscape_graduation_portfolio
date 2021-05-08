// Fill out your copyright notice in the Description page of Project Settings.


#include "PTTurret.h"
#include "PTEngineerProp.h"
#include "DrawDebugHelpers.h"
#include "PrototypeNPCCharacter.h"
#include "PTSoldierPassive_Projectile.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "PTPlayerController.h"
#include "PrototypeCharacter.h"
#include "Engine.h"
#include "Particles/ParticleSystem.h"


// Sets default values
APTTurret::APTTurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretHeadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretHeadMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TURRETHEAD(TEXT("StaticMesh'/Game/Meshes/Turret/TurretHead.TurretHead'"));
	if (TURRETHEAD.Succeeded())
	{
		TurretHeadMeshComponent->SetStaticMesh(TURRETHEAD.Object);
	}

	TurretBodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBodyMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TURRETBODY(TEXT("StaticMesh'/Game/Meshes/Turret/TurretBody.TurretBody'"));
	if (TURRETBODY.Succeeded())
	{
		TurretBodyMeshComponent->SetStaticMesh(TURRETBODY.Object);
	}


	RootComponent = TurretBodyMeshComponent;

	DetectSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphereComponent"));
	DetectSphereComponent->SetupAttachment(GetRootComponent());
	//터렛의 오브젝트 감지 범위
	DetectSphereComponent->SetSphereRadius(2000.0f);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(TurretHeadMeshComponent);
	TurretHeadMeshComponent->SetupAttachment(RootComponent);

	//터렛의 파티클과 사운드는 BP에서 동적으로 할당하도록 처리 함 (경로 설정 X)
	TurretParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("TurretParticle"));
	TurretFireSound = CreateDefaultSubobject<USoundBase>(TEXT("TurretFireSound"));
	
	//초기 상태 = 중립 상태
	CurrentTurretState = ETurretState::Neutral;
	this->Tags.Add(TEXT("Turret"));
	
	//감지된 적
	TargetNPC = nullptr;

	//감지된 적 수
	Count = 0;

	//해킹 완료 여부
	IsHackingCompleted = false;
	
	//발사 여부
	IsFiring = false;

	bReplicates = true;

}

// Called when the game starts or when spawned
void APTTurret::BeginPlay()
{
	Super::BeginPlay();
	BP_SoldierPassiveProjectlieClass = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/FirstPersonCPP/Blueprints/BP_PTSoldierPassive_Projectile.BP_PTSoldierPassive_Projectile_C"));
	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

// Called every frame
void APTTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//해킹 된 상태 즉 공격 가능한 상태
	if (CurrentTurretState == ETurretState::Activated)
	{
		//감지된 적의 수가 0명 일 경우(다 죽였을 경우)
		if (Count == 0)
		{
			//범위 내 모든 오브젝트 서치
			DetectSphereComponent->GetOverlappingActors(Targets);
			
			//오브젝트 수가 1명이라도 있을 경우
			if (Targets.Num() != 0)
			{
				//공격 상태로 전환
				CurrentTurretState = ETurretState::Attacking;
				Count = Targets.Num();
				DetectedTarget();
			}
			else
			{
				CurrentTurretState = ETurretState::Activated;
				return;
			}
		}
		else
		{
			CurrentTurretState = ETurretState::Activated;
			return;
		}
	}
}

//발사체 스폰 뿐만 아니라 포신의 방향도 리플리케이션 해야 하기 때문에 서버에서 실행 하도록 함
void APTTurret::Fire()
{
	if (CurrentTurretState == ETurretState::Attacking && IsFiring)
	{
		if (TargetNPC != nullptr)
		{
			//타겟이 파괴된 경우 
			if (TargetNPC->IsActorBeingDestroyed())
			{
				IsFiring = false;
				//남은 적 카운트 --;
				Count--;
				DetectedTarget();
			}
			else
			{
				
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				//터렛 헤드를 적을 향해 방향 전환
				FRotator Direction = UKismetMathLibrary::FindLookAtRotation(TurretHeadMeshComponent->GetComponentLocation(), TargetNPC->GetActorLocation());

				MuzzleLocation->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(MuzzleLocation->GetComponentLocation(), TargetNPC->GetActorLocation()));
				TurretHeadMeshComponent->SetWorldRotation(FRotator(Direction.Pitch, Direction.Yaw, 0.0f));

				//Fire 함수 자체가 서버에서 실행되기 때문에 발사체를 바로 스폰
				APTSoldierPassive_Projectile* Projectile = GetWorld()->SpawnActor<APTSoldierPassive_Projectile>(BP_SoldierPassiveProjectlieClass, MuzzleLocation->GetComponentLocation(), MuzzleLocation->GetComponentRotation(), ActorSpawnParams);
				
				//파티클 처리
				APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
				PlayerCharacter->SetTurretParticle(this);

				//터렛 발사 주기 0.25초
				GetWorld()->GetTimerManager().SetTimer(TimeHandler, this, &APTTurret::Fire, 0.25f, false);

			}
			
		}
	}

}

void APTTurret::PlayParticleAndSound()
{
	
	if ((nullptr != RootComponent) && (TurretParticle != nullptr) && (TurretFireSound != nullptr))
	{
		if (!(TurretParticle->IsLooping()))
		{
			UGameplayStatics::SpawnEmitterAttached(TurretParticle, MuzzleLocation, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);

		}
		if (!(TurretFireSound->IsLooping()))
		{
			UGameplayStatics::PlaySoundAtLocation(this, TurretFireSound, MuzzleLocation->GetComponentLocation());
		}
	}
	
}

//UI를 통해 해킹을 성공한 경우
void APTTurret::Hacking()
{
	if (CurrentTurretState == ETurretState::Neutral)
	{
		UE_LOG(LogTemp, Warning, TEXT("Turret Hacking "));
		CurrentTurretState = ETurretState::Activated;

		//클라이언트 동기화를 위해 서버에서도 처리
		APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
		PlayerCharacter->SetTurretHacking(this);

	}
	else
		return;
}

//적을 발견한 경우 공격 상태로 전환하기 위한 함수 / 공격 상태
void APTTurret::DetectedTarget()
{
	for (int i = Count; i > 0; i--)
	{
		//감지된 오브젝트의 수가 1이라도 있을 경우
		if (Count > 0)
		{
			//감지된 오브젝트들 중 하나의 목표물 설정
			TargetNPC = Targets[Count - 1];
			if (TargetNPC != nullptr)
			{
				//감지된 오브젝트가 적(벌레)가 맞을 경우
				if (TargetNPC->ActorHasTag(TEXT("NPC")))
				{
					//이미 목표를 지정하여 공격중이 아닌 경우를 체크
					if (!(IsFiring))
					{
						IsFiring = true;
						//발사 함수를 서버에서 실행
						APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
						PlayerCharacter->SetTurretFire(this, TargetNPC);
					}
				}
				//감지된 오브젝트가 적(벌레)가 아닌 경우 카운트-- 하여 제외
				else
				{
					Count--;
				}
			}
			//목표로 설정한 감지된 오브젝트가 제거 된 경우
			else
			{
				Count--;
			}
		}
		//감지된 오브젝트가 0일 경우 활성(대기) 상태로 전환
		else
		{
			Count = 0;
			CurrentTurretState = ETurretState::Activated;
			return;
		}

	}

}

void APTTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APTTurret , IsHackingCompleted);
	DOREPLIFETIME(APTTurret, CurrentTurretState);
	DOREPLIFETIME(APTTurret, IsFiring);
	DOREPLIFETIME(APTTurret, TargetNPC);
	DOREPLIFETIME(APTTurret, Count);
	DOREPLIFETIME(APTTurret, Targets);
	DOREPLIFETIME(APTTurret, TurretParticle);
	DOREPLIFETIME(APTTurret, TurretFireSound);
	DOREPLIFETIME(APTTurret, MuzzleLocation);

}