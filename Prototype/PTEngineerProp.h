// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTWeapon.h"
#include "PTEngineerProp.generated.h"

UCLASS()
class PROTOTYPE_API APTEngineerProp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTEngineerProp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetPassiveSkill(FVector StartPoint, FVector GetForwardVector, UWorld* World, EWeaponState CurrentState);

	UFUNCTION(BlueprintCallable)
	void SetActiveSkill(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter);

	UPROPERTY()
	bool IsFireSuccess;

	//중립 터렛에 접속 여부
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	bool IsTurretAccessed;

	//해킹 성공 여부
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	bool IsHackingCompleted;

	//퍼즐 힌트 넘버
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	TArray<int32> HintNumber;

	//스트링으로 변환
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	FString HintNumberToString;

	//정답
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	FString Answer;

	//현재 접속한 터렛
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Puzzle)
	AActor* CurrentTurret;


};
