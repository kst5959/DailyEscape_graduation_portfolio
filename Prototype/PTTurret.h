// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTTurret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Neutral UMETA(DisplayName = "Neutral"),
	Activated UMETA(DisplayName = "Activated"),
	Attacking UMETA(DisplayName = "Attacking")
};

UCLASS()
class PROTOTYPE_API APTTurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTTurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//�� ���� ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	class USphereComponent* DetectSphereComponent;

	//�ͷ� �Ӹ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Meshes)
	UStaticMeshComponent* TurretHeadMeshComponent;

	//�ͷ� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Meshes)
	UStaticMeshComponent* TurretBodyMeshComponent;

	//��ŷ ���� ��
	UFUNCTION(BlueprintCallable)
	void Hacking();

	//�� ���� �� ����
	UFUNCTION(BlueprintCallable)
	void DetectedTarget();

	//�ͷ� ���� ��ġ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret", Replicated)
	class USceneComponent* MuzzleLocation;

	//���� �ͷ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	ETurretState CurrentTurretState;

	FDamageEvent DamageEvent;

	//���� ������ Ÿ��
	UPROPERTY(Replicated)
	AActor* TargetNPC;

	//������ Ÿ�� ��
	UPROPERTY(Replicated)
	TArray<AActor*> Targets;

	//��ž �߻� �ֱ� Ÿ�̸�
	FTimerHandle TimeHandler;

	//���� ���� �� ��� ������Ʈ ��
	UPROPERTY(EditAnywhere, Replicated)
	int Count;

	//��ž �߻� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool IsFiring;

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(BlueprintCallable)
	void PlayParticleAndSound();

	//��ŷ �Ϸ� ���� -> �����Ͼ��������� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Turret, Replicated)
	bool IsHackingCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Turret, Replicated)
	UParticleSystem* TurretParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Turret, Replicated)
	USoundBase* TurretFireSound;

	UPROPERTY()
	APawn* Character; 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE ETurretState GetTurretState() { return CurrentTurretState; }

	FORCEINLINE bool GetIsHackingCompleted() { return IsHackingCompleted; }

	UPROPERTY(Category = "Turret", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBlueprintGeneratedClass* BP_SoldierPassiveProjectlieClass;

private:


};
