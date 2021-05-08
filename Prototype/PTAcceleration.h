// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTAcceleration.generated.h"

UCLASS()
class PROTOTYPE_API APTAcceleration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTAcceleration();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Acceleration)
	UBoxComponent* BoxComponent;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Acceleration(AActor* Object);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Accleration)
	float Duration;

};
