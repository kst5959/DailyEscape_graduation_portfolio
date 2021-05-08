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

	//�߸� �ͷ��� ���� ����
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	bool IsTurretAccessed;

	//��ŷ ���� ����
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	bool IsHackingCompleted;

	//���� ��Ʈ �ѹ�
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	TArray<int32> HintNumber;

	//��Ʈ������ ��ȯ
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	FString HintNumberToString;

	//����
	UPROPERTY(EdItAnywhere, BlueprintReadWrite, Category = Puzzle)
	FString Answer;

	//���� ������ �ͷ�
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Puzzle)
	AActor* CurrentTurret;


};
