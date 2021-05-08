// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "PrototypeAIController.h"
#include "PrototypeNPCCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "PrototypeCharacter.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
	DetectRadius = 2400.0f;
	IsFindTarget = false;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	FColor Color = FColor::Red;

	if (nullptr == ControllingPawn)
	{
		return;
	}

	UWorld* world = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	APrototypeNPCCharacter* ControllingNPC = Cast<APrototypeNPCCharacter>(ControllingPawn);

	if (nullptr == world)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParam(NAME_None, false, ControllingPawn);

	bool bResult = world->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionParam
	);

	if (bResult)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(APrototypeAIController::TargetKey, nullptr);
		for (FOverlapResult& OverlapResult : OverlapResults) 
		{
			APrototypeCharacter* PTCharacter = Cast<APrototypeCharacter>(OverlapResult.GetActor());

			if (PTCharacter != nullptr)
			{
				if (PTCharacter->GetController() != nullptr)
				{
					if (PTCharacter->GetController()->IsPlayerController())
					{
						//�÷��̾ ����ִ� ������ ���
						if (PTCharacter->GetPlayerState() != EPlayerState::Dead)
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(APrototypeAIController::TargetKey, PTCharacter);

							//�ش� ������ ���ظ� ������ �÷��̾ �ִ� ���
							if (ControllingNPC->GetDamageCauserPlayer() != nullptr)
							{
								//���� �÷��̾��� �Ÿ��� 1200 ������ ���
								if (ControllingNPC->GetDamageCauserPlayer()->GetDistanceTo(ControllingNPC) <= 1200.0f)
								{
									//������ ���ظ� ������ �÷��̾ �켱 Ÿ������ ����
									OwnerComp.GetBlackboardComponent()->SetValueAsObject(APrototypeAIController::TargetKey, ControllingNPC->GetDamageCauserPlayer());
								}
						
							}
							DrawDebugPoint(world, PTCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
							DrawDebugLine(world, ControllingPawn->GetActorLocation(), PTCharacter->GetActorLocation(), FColor::Black, false, 0.2f);
							Color = FColor::Purple;
							return;
						}
						else
							return;
					}
				}
			}
		}
	}
	if (ControllingNPC->GetIsHit())
	{
		SetDetectRange();
	}

	DrawDebugSphere(world, Center, DetectRadius, 16, Color, false, 0.2f);
	

}


void UBTService_Detect::SetDetectRange()
{
	DetectRadius = 3000.0f;
}

