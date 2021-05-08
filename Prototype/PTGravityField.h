// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTGravityField.generated.h"

UENUM(BlueprintType)
enum class EGravityFieldState : uint8
{
	Neutral UMETA(DisplayName = "Neutral"),
	MaxGravity UMETA(DisplayName = "MaxGravity"),
	ZeroGravity UMETA(DisplayName = "ZeroGravity"),

};

UCLASS()
class PROTOTYPE_API APTGravityField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTGravityField();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CheckingActors();

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void SetGravity(ACharacter* Character);

	UFUNCTION()
	void ChangeGravityFieldState();

	UFUNCTION()
	void ChangeCountDown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float mGravityDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float mChangeStateCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int32 mCountTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVector mSetCollisionBoxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float mGravityFieldDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EGravityFieldState CurrentGravityFieldState;

	FTimerHandle TimerHandle;

	FORCEINLINE EGravityFieldState GetGravityFieldState() { return CurrentGravityFieldState; }
};
