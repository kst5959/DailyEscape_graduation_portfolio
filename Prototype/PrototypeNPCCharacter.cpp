// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeNPCCharacter.h"
#include "PrototypeAIController.h"
#include "PrototypeCharacter.h"
#include "PTPlayerController.h"
#include "NPCAniminstance.h"
#include "DrawDebugHelpers.h"
#include "BTDecorator_IsInAttackRange.h"
#include "BTService_Detect.h"
#include "Net/UnrealNetwork.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
APrototypeNPCCharacter::APrototypeNPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = GetCapsuleComponent();

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, 0.0f, 0.0f));
	GetMesh()->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial>SLOW_MATERIAL(TEXT("/Game/Material/SlowMaterial.SlowMaterial"));
	if (SLOW_MATERIAL.Succeeded())
	{
		SlowMaterial = SLOW_MATERIAL.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>CHARACTER_MATERIAL(TEXT("/Game/BugModeling/BugModelFinal/Material__1016.Material__1016"));
	if (CHARACTER_MATERIAL.Succeeded())
	{
		CharacterMaterial = CHARACTER_MATERIAL.Object;
	}

	DefenderActiveParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("DefenderActiveParticle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefenderActive_Particle(TEXT("/Game/Particle/P_Skill_telecharge_Shock_Charge_01.P_Skill_telecharge_Shock_Charge_01"));
	if (DefenderActive_Particle.Succeeded())
	{
		DefenderActiveParticle = DefenderActive_Particle.Object;
	}

	mSlowCountTime = 5.0f;
	mHP = 100.0f;

	bIsHit = false;
	bIsAttacking = false;

	mFinalDamage = 0.0f;

	DamageCauserPlayer = nullptr;

	PTAIController = nullptr;

	Character = nullptr;

	bReplicates = true;
}

void APrototypeNPCCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
		
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

}

// Called when the game starts or when spawned
void APrototypeNPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AIControllerClass = APrototypeAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	APTPlayerController* AIController = Cast<APTPlayerController>(GetController());

	if (AIController != nullptr)
	{
		PTAIController = AIController;
	}
	
	//Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

}

// Called every frame
void APrototypeNPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSlow)
	{
		mSlowCountTime = mSlowCountTime - (1.0f * DeltaTime);
		if (mSlowCountTime <= 0.0f)
		{
			Slow();
		}
	}
	
}


//플레이어 공격
void APrototypeNPCCharacter::AttackCheck()
{
	bIsAttacking = true;

	if (bIsAttacking)
	{
		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params(NAME_None, false, this);
		Params.AddIgnoredActor(this);
		FVector Center = this->GetActorLocation();
	
		bool bResult = this->GetWorld()->SweepMultiByChannel(
			HitResults,
			Center,
			Center + GetActorForwardVector() * 100.0f,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel15,
			FCollisionShape::MakeSphere(50.0f),
			Params
		);
	
		if (bResult)
		{
			for (FHitResult& HitResult : HitResults)
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Player")))
				{	
					FDamageEvent DamageEvent;
					HitResult.GetActor()->TakeDamage(20.0f, DamageEvent, GetController(), this);	
					
					UE_LOG(LogTemp, Warning, TEXT("Call NPC AttackCheck %s"), bIsAttacking ? TEXT("true") : TEXT("false"));
				}
			}
		}

		OnAttackEnd.Broadcast();
	}
}


float APrototypeNPCCharacter::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (this != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("In NPC Actor : %s took Damage : %f HP : %f"), *GetName(), DamageAmount, mHP);
		
		if (DamageCauser->ActorHasTag(TEXT("Player")))
		{
			//플레이어의 플레이어 컨트롤러로 가서 처리
			APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(DamageCauser);
			PlayerCharacter->SetNPCHP(this, DamageAmount);
		}

		return DamageAmount;
	}
	else
		return 0.0f;

}

void APrototypeNPCCharacter::SetHP(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Set HP "));
	
	if (this != nullptr)
	{
		mHP = mHP - Damage;
		if (mHP <= 0)
		{
			Destroy();
		}
	}
	
}

void APrototypeNPCCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APrototypeNPCCharacter, mHP);
	DOREPLIFETIME(APrototypeNPCCharacter, PTAIController);
	DOREPLIFETIME(APrototypeNPCCharacter, bIsHit);
	DOREPLIFETIME(APrototypeNPCCharacter, bIsSlow);
	DOREPLIFETIME(APrototypeNPCCharacter, mSlowCountTime);
	DOREPLIFETIME(APrototypeNPCCharacter, SlowMaterial);
	DOREPLIFETIME(APrototypeNPCCharacter, CharacterMaterial);
	DOREPLIFETIME(APrototypeNPCCharacter, bIsAttacking);
	DOREPLIFETIME(APrototypeNPCCharacter, DamageCauserPlayer);
	DOREPLIFETIME(APrototypeNPCCharacter, DefenderActiveParticle);

}

bool APrototypeNPCCharacter::GetIsHit()
{
	return this->bIsHit;
}

void APrototypeNPCCharacter::SetIsHit(bool bHit)
{
	this->bIsHit = bHit;
}

bool APrototypeNPCCharacter::GetIsAttaking()
{
	return this->bIsAttacking;
}

void APrototypeNPCCharacter::SetIsAttaking(bool bAttacking)
{
	this->bIsAttacking = bAttacking;
}


//닥터 패시브 맞을 경우 슬로우 및 머티리얼 변경
void APrototypeNPCCharacter::Slow()
{
	if (this != nullptr)
	{
		if (!bIsSlow)
		{
			this->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
			this->GetMesh()->SetMaterial(0, SlowMaterial);
			bIsSlow = true;
		}
		else if (bIsSlow && mSlowCountTime <= 0)
		{
			this->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
			this->GetMesh()->SetMaterial(0, CharacterMaterial);
			mSlowCountTime = 5.0f;
			bIsSlow = false;
		}
	}
}

//디펜더 액티브 맞을 경우 파티클 재생
void APrototypeNPCCharacter::PlayDefenderActiveParticle()
{
	UGameplayStatics::SpawnEmitterAttached(DefenderActiveParticle, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
}
