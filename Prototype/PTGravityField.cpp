// Fill out your copyright notice in the Description page of Project Settings.


#include "PTGravityField.h"
#include "Components/BoxComponent.h"
#include "PrototypeNPCCharacter.h"
#include "PrototypeGameMode.h"
#include "Engine.h"
#include "PrototypeCharacter.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : �÷��̾ Ư�� ���� ���� �� �߷� ���� ȿ���� �ֱ� ���� Ŭ����
* @updater : Ȳ����
* @update : �ڵ� �����丵 : ���� �� / �� ������ ���� ����
*/
APTGravityField::APTGravityField()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	//�߷��� ũ��
	mSetCollisionBoxSize = FVector(400.0f, 400.0f, 400.0f);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionComponent->SetBoxExtent(mSetCollisionBoxSize, false);
	CollisionComponent->SetCollisionProfileName("Trigger");

	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &APTGravityField::OnOverlapEnd);

	CollisionComponent->SetVisibility(true);

	mChangeStateCount = 5.0f;
	mGravityDuration = 10.0f;
	mCountTime = 5;
	mGravityFieldDamage = 5.0f;

	CurrentGravityFieldState = EGravityFieldState::Neutral;
}

// Called when the game starts or when spawned
void APTGravityField::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &APTGravityField::ChangeCountDown, 1.0f, true);
	
}

// Called every frame
void APTGravityField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetGravityFieldState() != EGravityFieldState::Neutral)
	{
		CheckingActors();
	}

	if (mChangeStateCount <= 0)
	{
		ChangeGravityFieldState();
	}
	mChangeStateCount--;
	

}

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : �߷��� ������ ���� �� �߷� ���¸� ������� �ǵ���
* @updater : Ȳ����
* @update : 
*/
void APTGravityField::OnOverlapEnd(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{
		if (OtherActor->ActorHasTag(TEXT("NPC")))
		{
			//UE_LOG(LogTemp, Warning, TEXT("END"));
			ACharacter* Character = Cast<ACharacter>(OtherActor);															
			Character->GetCharacterMovement()->GravityScale = 1.0f;

		}
		else if (OtherActor->ActorHasTag(TEXT("Player")))
		{
			APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(OtherActor);
			if (PlayerCharacter->GetPlayerState() == EPlayerState::InMaxGravity || PlayerCharacter->GetPlayerState() == EPlayerState::InZeroGravity)
			{
				PlayerCharacter->CurrentPlayerState = EPlayerState::Alive;
			}
			PlayerCharacter->GetCharacterMovement()->GravityScale = 1.0f;
		}
	}
}

void APTGravityField::CheckingActors()
{
	UWorld* world = GetWorld();
	TArray<FOverlapResult> OverlapResults;
	uint8 BoxDepth = 0;

	FCollisionQueryParams CollisionParam(NAME_None, false, this);
	FVector Center = this->GetActorLocation();
	FColor Color = FColor::Yellow;

	bool bResult = world->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel6,
		FCollisionShape::MakeBox(FVector(300.0f, 300.0f, 300.0f)),
		CollisionParam
	);

	if (bResult)
	{
		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			if (OverlapResult.GetActor() != nullptr)
			{
				if (OverlapResult.GetActor()->ActorHasTag(TEXT("Player")) || OverlapResult.GetActor()->ActorHasTag(TEXT("NPC")))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Interact"));
					ACharacter* Character = Cast<ACharacter>(OverlapResult.GetActor());
					SetGravity(Character);
				}
			}
		}
	}

	DrawDebugBox(world, Center, FVector(300.0f, 300.0f, 300.0f), Color , false, 0.5f, BoxDepth, 3.0f);
	//DrawDebugSphere(world, Center, 600.0f, 16, Color, false, 0.5f);
}

/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : �߷��� �� ���¿� ���� �÷��̾� �� NPC�� ���� ����
* @updater : Ȳ����
* @update :
*/
void APTGravityField::SetGravity(ACharacter* Character)
{
	if (GetGravityFieldState() == EGravityFieldState::ZeroGravity)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Set ZeroGravity"));
		Character->GetCharacterMovement()->GravityScale = 0.0f;

		if (Character->ActorHasTag(TEXT("Player")))
		{
			APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
			if (PlayerCharacter->GetPlayerState() == EPlayerState::Alive || PlayerCharacter->GetPlayerState() == EPlayerState::InMaxGravity)
			{
				PlayerCharacter->CurrentPlayerState = EPlayerState::InZeroGravity;
			}
		}
	}

	else if (GetGravityFieldState() == EGravityFieldState::MaxGravity)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Set MaxGravity"));
		if (Character->ActorHasTag(TEXT("Player")))
		{
			APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
			if (PlayerCharacter->GetPlayerState() == EPlayerState::Alive || PlayerCharacter->GetPlayerState() == EPlayerState::InZeroGravity)
			{
				PlayerCharacter->CurrentPlayerState = EPlayerState::InMaxGravity;
			}
			if (GetLocalRole() == ROLE_Authority)
			{
				UGameplayStatics::ApplyDamage(Character, mGravityFieldDamage, Character->GetController(), this, UDamageType::StaticClass());
			}
		}
		Character->GetCharacterMovement()->GravityScale = 5.0f;
	}

	else if (GetGravityFieldState() == EGravityFieldState::Neutral)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Set NeutralGravity"));
		if (Character->ActorHasTag(TEXT("Player")))
		{
			APrototypeCharacter* PlayerCharacter = Cast<APrototypeCharacter>(Character);
			if (PlayerCharacter->GetPlayerState() == EPlayerState::InMaxGravity)
			{
				PlayerCharacter->CurrentPlayerState = EPlayerState::Alive;
			}
			else if (PlayerCharacter->GetPlayerState() == EPlayerState::InZeroGravity)
			{
				PlayerCharacter->CurrentPlayerState = EPlayerState::Alive;
			}
		}
	}
		

}

void APTGravityField::ChangeGravityFieldState()
{
	//UE_LOG(LogTemp, Warning, TEXT("ChangeGravityFieldState"));

	if (GetGravityFieldState() == EGravityFieldState::MaxGravity)
	{
		CurrentGravityFieldState = EGravityFieldState::ZeroGravity;
	}
	
	else if (GetGravityFieldState() == EGravityFieldState::ZeroGravity)
	{
		CurrentGravityFieldState = EGravityFieldState::Neutral;
	}

	else if (GetGravityFieldState() == EGravityFieldState::Neutral)
	{
		CurrentGravityFieldState = EGravityFieldState::MaxGravity;
	}

	mChangeStateCount = 5;
}


/**
* @author : Ȳ����
* @date : 2021-05-02
* @description : �߷��� �� ������ ���ӽð� ī��Ʈ : 5��
* @updater : Ȳ����
* @update :
*/
void APTGravityField::ChangeCountDown()
{
	--mCountTime;
	CheckingActors();

	if (mCountTime < 1)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		mCountTime = 5;
		ChangeGravityFieldState();

	}
}

