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
	//�ͷ��� ������Ʈ ���� ����
	DetectSphereComponent->SetSphereRadius(2000.0f);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(TurretHeadMeshComponent);
	TurretHeadMeshComponent->SetupAttachment(RootComponent);

	//�ͷ��� ��ƼŬ�� ����� BP���� �������� �Ҵ��ϵ��� ó�� �� (��� ���� X)
	TurretParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("TurretParticle"));
	TurretFireSound = CreateDefaultSubobject<USoundBase>(TEXT("TurretFireSound"));
	
	//�ʱ� ���� = �߸� ����
	CurrentTurretState = ETurretState::Neutral;
	this->Tags.Add(TEXT("Turret"));
	
	//������ ��
	TargetNPC = nullptr;

	//������ �� ��
	Count = 0;

	//��ŷ �Ϸ� ����
	IsHackingCompleted = false;
	
	//�߻� ����
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

	//��ŷ �� ���� �� ���� ������ ����
	if (CurrentTurretState == ETurretState::Activated)
	{
		//������ ���� ���� 0�� �� ���(�� �׿��� ���)
		if (Count == 0)
		{
			//���� �� ��� ������Ʈ ��ġ
			DetectSphereComponent->GetOverlappingActors(Targets);
			
			//������Ʈ ���� 1���̶� ���� ���
			if (Targets.Num() != 0)
			{
				//���� ���·� ��ȯ
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

//�߻�ü ���� �Ӹ� �ƴ϶� ������ ���⵵ ���ø����̼� �ؾ� �ϱ� ������ �������� ���� �ϵ��� ��
void APTTurret::Fire()
{
	if (CurrentTurretState == ETurretState::Attacking && IsFiring)
	{
		if (TargetNPC != nullptr)
		{
			//Ÿ���� �ı��� ��� 
			if (TargetNPC->IsActorBeingDestroyed())
			{
				IsFiring = false;
				//���� �� ī��Ʈ --;
				Count--;
				DetectedTarget();
			}
			else
			{
				
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				//�ͷ� ��带 ���� ���� ���� ��ȯ
				FRotator Direction = UKismetMathLibrary::FindLookAtRotation(TurretHeadMeshComponent->GetComponentLocation(), TargetNPC->GetActorLocation());

				MuzzleLocation->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(MuzzleLocation->GetComponentLocation(), TargetNPC->GetActorLocation()));
				TurretHeadMeshComponent->SetWorldRotation(FRotator(Direction.Pitch, Direction.Yaw, 0.0f));

				//Fire �Լ� ��ü�� �������� ����Ǳ� ������ �߻�ü�� �ٷ� ����
				APTSoldierPassive_Projectile* Projectile = GetWorld()->SpawnActor<APTSoldierPassive_Projectile>(BP_SoldierPassiveProjectlieClass, MuzzleLocation->GetComponentLocation(), MuzzleLocation->GetComponentRotation(), ActorSpawnParams);
				
				//��ƼŬ ó��
				APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
				PlayerCharacter->SetTurretParticle(this);

				//�ͷ� �߻� �ֱ� 0.25��
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

//UI�� ���� ��ŷ�� ������ ���
void APTTurret::Hacking()
{
	if (CurrentTurretState == ETurretState::Neutral)
	{
		UE_LOG(LogTemp, Warning, TEXT("Turret Hacking "));
		CurrentTurretState = ETurretState::Activated;

		//Ŭ���̾�Ʈ ����ȭ�� ���� ���������� ó��
		APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
		PlayerCharacter->SetTurretHacking(this);

	}
	else
		return;
}

//���� �߰��� ��� ���� ���·� ��ȯ�ϱ� ���� �Լ� / ���� ����
void APTTurret::DetectedTarget()
{
	for (int i = Count; i > 0; i--)
	{
		//������ ������Ʈ�� ���� 1�̶� ���� ���
		if (Count > 0)
		{
			//������ ������Ʈ�� �� �ϳ��� ��ǥ�� ����
			TargetNPC = Targets[Count - 1];
			if (TargetNPC != nullptr)
			{
				//������ ������Ʈ�� ��(����)�� ���� ���
				if (TargetNPC->ActorHasTag(TEXT("NPC")))
				{
					//�̹� ��ǥ�� �����Ͽ� �������� �ƴ� ��츦 üũ
					if (!(IsFiring))
					{
						IsFiring = true;
						//�߻� �Լ��� �������� ����
						APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
						PlayerCharacter->SetTurretFire(this, TargetNPC);
					}
				}
				//������ ������Ʈ�� ��(����)�� �ƴ� ��� ī��Ʈ-- �Ͽ� ����
				else
				{
					Count--;
				}
			}
			//��ǥ�� ������ ������ ������Ʈ�� ���� �� ���
			else
			{
				Count--;
			}
		}
		//������ ������Ʈ�� 0�� ��� Ȱ��(���) ���·� ��ȯ
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