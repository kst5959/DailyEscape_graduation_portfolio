// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAnimInstance.h"
#include "PrototypeNPCCharacter.h"

UNPCAnimInstance::UNPCAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
}

void UNPCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}