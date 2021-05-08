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

	//적 감지 범위 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	class USphereComponent* DetectSphereComponent;

	//터렛 머리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Meshes)
	UStaticMeshComponent* TurretHeadMeshComponent;

	//터렛 몸
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Meshes)
	UStaticMeshComponent* TurretBodyMeshComponent;

	//해킹 성공 시
	UFUNCTION(BlueprintCallable)
	void Hacking();

	//적 감지 및 공격
	UFUNCTION(BlueprintCallable)
	void DetectedTarget();

	//터렛 포신 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret", Replicated)
	class USceneComponent* MuzzleLocation;

	//현재 터렛 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	ETurretState CurrentTurretState;

	FDamageEvent DamageEvent;

	//현재 공격할 타겟
	UPROPERTY(Replicated)
	AActor* TargetNPC;

	//감지된 타겟 들
	UPROPERTY(Replicated)
	TArray<AActor*> Targets;

	//포탑 발사 주기 타이머
	FTimerHandle TimeHandler;

	//감지 범위 내 모든 오브젝트 수
	UPROPERTY(EditAnywhere, Replicated)
	int Count;

	//포탑 발사 여부 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool IsFiring;

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(BlueprintCallable)
	void PlayParticleAndSound();

	//해킹 완료 여부 -> 엔지니어프롭으로 전달
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
