// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "PTWeapon.h"
#include "GameFramework/Character.h"
#include "PrototypeCharacter.generated.h"

class UInputComponent;

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Alive UMETA(DisplayName = "Alive"),
	Groggy UMETA(DisplayName = "Groggy"),
	Dead UMETA(DisplayName = "Dead"),
	InMaxGravity UMETA(DisplayName = "InMaxGravity"),
	InZeroGravity UMETA(DisplayName = "InZeroGravity")
};

UCLASS(config=Game)
class APrototypeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APrototypeCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay();

public:

	UFUNCTION(BlueprintCallable)
	void ActiveSkill();

	UFUNCTION()
	void Surf();

	UFUNCTION()
	void StopSurf();

	UFUNCTION()
	void Running();

	UFUNCTION()
	void StopRunning();
	
	//벽 타기 캡슐컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "CensorWall", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CensoreCapsuleComponent;

	//벽 타기 상태 관리
	UPROPERTY(VisibleAnywhere, Category = "CensorWall")
	int WallCounter;

	//땅에 있는지.
	UFUNCTION()
	bool OnGround();

	//벽에 닿았을 경우
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//벽에서 떨어졌을 경우
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//1인칭 FPP 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	//3인칭 TPP 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BodyMesh;

	//넉백건
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, Replicated, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FP_Gun;

	//3인칭 스켈레탈 웨폰 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ThirdPersonSkeletalMeshWeapon;

	//3인칭 스태틱 웨폰 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ThirdPersonStaticMeshWeapon;

	//1인칭 스태틱 웨폰 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshWeaponComponent;

	//총구 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* TP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Replicated)
	FVector MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Replicated)
	FRotator MuzzleRotation;

	//카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Replicated)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Replicated)
	float Stamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Replicated)
	float Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	bool CanJumping = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bPressedJumping;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Replicated)
	bool bPressedRun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bPrssedSurf;

	//피격 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//클래스에 맞는 패시브 설정
	UFUNCTION(BlueprintCallable, Category = "Passive")
	void SetPlayerPassive();

	UFUNCTION(BlueprintCallable, Category = "Passive")
	void SetPlayerWeaponMeshSocket();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive", Replicated)
	float SoldierPassive;

	//기본 이속
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Replicated)
	float PlayerWalkSpeed;

	//달리기 이속
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Replicated)
	float PlayerRunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive", Replicated)
	float DefenderArmorPassive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Replicated)
	float ArmorRecovery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive", Replicated)
	float DoctorPassive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Replicated)
	float StaminaRecovery;


	//피격
	UFUNCTION(BlueprintCallable)
	void HPChange(float Damage);

	//피격
	UFUNCTION(BlueprintCallable)
	void ArmorChange();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	//총구 위치 조정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* KnockbackFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoldierPassiveFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* SoldierActiveFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* DoctorPassiveFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* ReloadingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = Particle)
	class UParticleSystem* KnockbackFireParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
	class UParticleSystem* SoldierPassiveFireParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
	class UParticleSystem* SoldierActiveFireParticle;

	UFUNCTION(BlueprintCallable)
	void PlayAnimation();

	UFUNCTION(BlueprintCallable)
	void IsJump();

	//넉백건 변경
	UFUNCTION(BlueprintCallable)
	void ChangeKnockbackGun();

	//클래스 웨폰 변경
	UFUNCTION(BlueprintCallable)
	void ChangeClassWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APTWeapon* PTWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APTFireArm* PTFireArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APTEngineerProp* PTEngineerProp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APTEquipment* PTEquipment;

	UPROPERTY(BlueprintReadOnly,Replicated)
	EWeaponState CurrentWeaponState;

	UPROPERTY()
	TSubclassOf<class APTFireArm> PTFireArmClass; 

	UPROPERTY()
	TSubclassOf<class APTWeapon> PTWeaponClass;

	UPROPERTY()
	TSubclassOf<class APTEquipment> PTEquipmentClass;

	UPROPERTY()
	TSubclassOf<class APTEngineerProp> PTEngineerPropClass;

	//디펜더 아머 충전 스킬
	UFUNCTION(BlueprintCallable)
	void SetArmor(float NewArmor, bool bUsing);

	//디펜더 스킬 사용 체크
	UPROPERTY()
	bool IsUsingDefenderSkill;

	//액티브 스킬 사용 체크
	UPROPERTY()
	bool IsUsingActiveSkill;

	//연사모드일 경우
	UPROPERTY(Replicated)
	bool IsFiring;

	UFUNCTION()
	void StartFire();

	UFUNCTION()
	void StopFire();

	UFUNCTION(BlueprintCallable)
	void Reloading();

	//연사모드 타이머
	FTimerHandle timer;

	//그로기상태일때 닥터스킬로 소생
	UFUNCTION(BlueprintCallable)
	void SetRevive();

	//그로기
	UFUNCTION(BlueprintCallable)
	void SetGroggy();

	//사망
	UFUNCTION(BlueprintCallable)
	void SetDead();

	//현재 플레이어 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
	EPlayerState CurrentPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat" , Replicated)
	float ActiveSkillCooldown;

	//엔지니어 가속 관문
	UFUNCTION(BlueprintCallable)
	void Acceleration();

	//가속 관문 감속
	UFUNCTION(BlueprintCallable)
	void Deceleration();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated)
	bool IsEnterdAccelerationField;

	//아머에서 차감 후 hp에서 받을 데미지
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly,Replicated)
	float CurrentDamage;

	//감속 카운트
	UPROPERTY(Replicated)
	int32 DecelerationCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
	bool IsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPTAnimInstance* BodyMeshAnimInstance;

	virtual void PostInitializeComponents();
	
	UFUNCTION(BlueprintCallable)
	void PlaySoundParticle();
	
	UPROPERTY(Replicated)
	float DoctorHeal;

	UFUNCTION(BlueprintCallable)
	void SetPlayTPPSoundAndParticle(UParticleSystem* Particle, USoundBase* Sound);

	UFUNCTION(BlueprintCallable)
	void SetPlayFPPSoundAndParticle(UParticleSystem* Particle, USoundBase* Sound);

	FTimerHandle StaminaTimeHandler;

	// 밑으로는 RPC함수들

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void SetNPCHP(AActor* TargetActor, float Damage);

	UFUNCTION()
	void ReplicateJumpStamina();
	
	UFUNCTION()
	void ReplicateActiveCooldown();

	UFUNCTION(Server, Reliable)
	void SetWalkSpeed(float Speed, bool RunState);

	UFUNCTION(Server, Reliable)
	void SetSurf(float Speed, bool SurfState);

	UFUNCTION(Server, Reliable)
	void SetTurretHacking(class APTTurret* Turret);

	UFUNCTION(Server, Reliable)
	void SetTurretFire(class APTTurret* Turret, AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void SetTurretParticle(class APTTurret* Turret);

	UFUNCTION(Server, Reliable)
	void SetAcceleration();

	UFUNCTION(NetMulticast, Reliable)
	void SetDeceleration();

	UFUNCTION(Server, Reliable)
	void SetDoctorActive(FVector PlayerLocation, FRotator PlayerRotation, UWorld* World, APTEquipment* Equipment);

	UFUNCTION(Server, Reliable)
	void SetDefenderPassive(APrototypeCharacter* PlayerCharacter,float Shield , bool bUsingSkill);

	UFUNCTION(Server, Reliable)
	void SetDefenderActive(FVector PlayerLocation, FRotator PlayerRotation, UWorld* World, APTEquipment* Equipment);

	UFUNCTION(NetMulticast, Reliable)
	void PlayDefenderActiveParticle(APrototypeNPCCharacter* TargetActor);

	/////////////////아래부터 RPC 액터 스폰///////////////////////
	UFUNCTION(Server, Reliable)
	void SetSpawnSoldierActiveProjectile();

	UFUNCTION(Server, Reliable)
	void SetSpawnSoldierPassiveProjectile();
	
	UFUNCTION(Server, Reliable)
	void SetSpawnKnockbackProjectile();

	UFUNCTION(Server, Reliable)
	void SetSpawnDefenderShield();

	UFUNCTION(Server, Reliable)
	void SetSpawnDoctorPassiveProjectile();

	UFUNCTION(Server, Reliable)
	void SetSpawnEngineerActive();


protected:

	void OnFire();

	void MoveForward(float Val);

	void MoveRight(float Val);
	
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	FORCEINLINE class USkeletalMeshComponent* GetBodyMesh() const { return BodyMesh; }

	FORCEINLINE class UPTAnimInstance* GetBodyAniminstance() const { return BodyMeshAnimInstance; }

	FORCEINLINE bool GetIsFiring() const { return IsFiring; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetFPGun() const { return FP_Gun; }

	FORCEINLINE class USceneComponent* GetMuzzleLocation() const { return FP_MuzzleLocation; }

	FORCEINLINE class USceneComponent* GetThirdPersonMuzzleLocation() const { return TP_MuzzleLocation; }

	FORCEINLINE class UStaticMeshComponent* GetStaticWeaponMesh() const { return StaticMeshWeaponComponent; }

	FORCEINLINE class USkeletalMeshComponent* GetThirdPersonSkeletalMeshWeapon() const { return ThirdPersonSkeletalMeshWeapon; }

	FORCEINLINE class UStaticMeshComponent* GetThirdPersonStaticMeshWeapon() const { return ThirdPersonStaticMeshWeapon; }

	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	FORCEINLINE EPlayerState GetPlayerState() { return CurrentPlayerState; }

};

