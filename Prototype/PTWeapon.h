// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PTWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	KnockbackGun UMETA(DisplayName = "KnockbackGun"),
	SoldierWeapon UMETA(DisplayName = "SoldierWeapon"),
	DefenderWeapon UMETA(DisplayName = "DefenderWeapon"),
	DoctorWeapon UMETA(DisplayName = "DoctorWeapon"),
	EngineerWeapon UMETA(DisplayName = "EngineerWeapon")
};

UCLASS()
class PROTOTYPE_API APTWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APTWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = "WeapoonMeshes", EditAnywhere, BlueprintReadWrite ,Replicated)
	USkeletalMesh* KnockbackGunMesh;

	UPROPERTY(Category = "WeaponMeshes", EditAnywhere, BlueprintReadWrite, Replicated)
	USkeletalMesh* SoldierWeaponMesh;


	UPROPERTY(Category = "WeaponMeshes", EditAnywhere, BlueprintReadWrite, Replicated)
	UStaticMesh* DoctorStaticMesh;

	UPROPERTY(Category = "WeaponMeshes", EditAnywhere, BlueprintReadWrite, Replicated)
	UStaticMesh* DefenderStaticMesh;

	UPROPERTY(Category = "WeaponMeshes", EditAnywhere, BlueprintReadWrite, Replicated)
	UStaticMesh* EngineerStaticMesh;

	//1인칭 메쉬 변경
	UFUNCTION(BlueprintCallable)
	void ChangeKnockbackGun(class APrototypeCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void ChangeClassWeapon(class APrototypeCharacter* Character);


	//타 플레이어 시점에서의 3인칭 메쉬 변경
	UFUNCTION(BlueprintCallable)
	void ThirdPersonChangeKnockbackGun(class APrototypeCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void ThirdPersonChangeClassWeapon(class APrototypeCharacter* Character);


	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
	EWeaponState CurrentWeaponState;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE EWeaponState GetWeaponState() { return CurrentWeaponState; }

};
