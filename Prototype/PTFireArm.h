// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "PTWeapon.h"
#include "GameFramework/Actor.h"
#include "PTFireArm.generated.h"

UENUM()
enum class EFireMode : uint8
{
	Auto UMETA(DisplayName = "Auto"),
	Burst UMETA(DisplayName = "Burst"),
	SemiAuto UMETA(DisplayName = "SemiAuto"),
	Charge UMETA(DisplayName = "Charge")
};

UCLASS()
class PROTOTYPE_API APTFireArm : public APTWeapon
{
	GENERATED_BODY()

public:
	APTFireArm();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Fire(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	void SetActiveSkill(EWeaponState CurrentState, APrototypeCharacter* PlayerCharacter);

	UPROPERTY()
	bool IsFireSuccess;

	FORCEINLINE EFireMode GetFireMode() { return CurrentFireMode; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int CurrentKnockbackAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int MaxKnockbackAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int CurrentSoldierAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int MaxKSoldierAmmo;


private:

	UPROPERTY(Category = Firearm, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EFireMode CurrentFireMode;

};
