// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class PROTOTYPE_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();

	UFUNCTION(BlueprintCallable)
	void SpawnAlien();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,meta = (AllowPrivateAccess = true), Category = "SpawnSetting")
	UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UBlueprintGeneratedClass* BP_PrototypeNPCCharacter;

	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "SpawnSetting")
	int64 mSetSpawnVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "SpawnSetting")
	FVector mSetCollisionBoxSize;

};
