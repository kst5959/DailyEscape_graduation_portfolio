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
	
	//�� Ÿ�� ĸ��������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "CensorWall", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CensoreCapsuleComponent;

	//�� Ÿ�� ���� ����
	UPROPERTY(VisibleAnywhere, Category = "CensorWall")
	int WallCounter;

	//���� �ִ���.
	UFUNCTION()
	bool OnGround();

	//���� ����� ���
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//������ �������� ���
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//1��Ī FPP �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh1P;

	//3��Ī TPP �޽�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BodyMesh;

	//�˹��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, Replicated, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FP_Gun;

	//3��Ī ���̷�Ż ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ThirdPersonSkeletalMeshWeapon;

	//3��Ī ����ƽ ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ThirdPersonStaticMeshWeapon;

	//1��Ī ����ƽ ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshWeaponComponent;

	//�ѱ� 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* TP_MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Replicated)
	FVector MuzzleLocation;

	UPROPERTY(VisibleDefaultsOnly, Replicated)
	FRotator MuzzleRotation;

	//ī�޶� ������Ʈ
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

	//�ǰ� �Լ�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//Ŭ������ �´� �нú� ����
	UFUNCTION(BlueprintCallable, Category = "Passive")
	void SetPlayerPassive();

	UFUNCTION(BlueprintCallable, Category = "Passive")
	void SetPlayerWeaponMeshSocket();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive", Replicated)
	float SoldierPassive;

	//�⺻ �̼�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Replicated)
	float PlayerWalkSpeed;

	//�޸��� �̼�
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


	//�ǰ�
	UFUNCTION(BlueprintCallable)
	void HPChange(float Damage);

	//�ǰ�
	UFUNCTION(BlueprintCallable)
	void ArmorChange();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	//�ѱ� ��ġ ����
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

	//�˹�� ����
	UFUNCTION(BlueprintCallable)
	void ChangeKnockbackGun();

	//Ŭ���� ���� ����
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

	//����� �Ƹ� ���� ��ų
	UFUNCTION(BlueprintCallable)
	void SetArmor(float NewArmor, bool bUsing);

	//����� ��ų ��� üũ
	UPROPERTY()
	bool IsUsingDefenderSkill;

	//��Ƽ�� ��ų ��� üũ
	UPROPERTY()
	bool IsUsingActiveSkill;

	//�������� ���
	UPROPERTY(Replicated)
	bool IsFiring;

	UFUNCTION()
	void StartFire();

	UFUNCTION()
	void StopFire();

	UFUNCTION(BlueprintCallable)
	void Reloading();

	//������ Ÿ�̸�
	FTimerHandle timer;

	//�׷α�����϶� ���ͽ�ų�� �һ�
	UFUNCTION(BlueprintCallable)
	void SetRevive();

	//�׷α�
	UFUNCTION(BlueprintCallable)
	void SetGroggy();

	//���
	UFUNCTION(BlueprintCallable)
	void SetDead();

	//���� �÷��̾� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
	EPlayerState CurrentPlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat" , Replicated)
	float ActiveSkillCooldown;

	//�����Ͼ� ���� ����
	UFUNCTION(BlueprintCallable)
	void Acceleration();

	//���� ���� ����
	UFUNCTION(BlueprintCallable)
	void Deceleration();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated)
	bool IsEnterdAccelerationField;

	//�Ƹӿ��� ���� �� hp���� ���� ������
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly,Replicated)
	float CurrentDamage;

	//���� ī��Ʈ
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

	// �����δ� RPC�Լ���

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

	/////////////////�Ʒ����� RPC ���� ����///////////////////////
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

