// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PrototypeCharacter.h"
#include "PrototypeGameMode.h"
#include "PTPlayerState.h"
#include "Animation/AnimInstance.h"
#include "PrototypeNPCCharacter.h"
#include "PrototypeProjectile_Soldier.h"
#include "PTPlayerController.h"
#include "PTAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PTWeapon.h"
#include "PTEquipment.h"
#include "PTFireArm.h"
#include "PTTurret.h"
#include "PTEngineerProp.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"
#include "Engine/EngineTypes.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APrototypeCharacter

APrototypeCharacter::APrototypeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	UWorld* World = GetWorld();

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	PTFireArmClass = APTFireArm::StaticClass();
	PTWeaponClass = APTWeapon::StaticClass();
	PTEquipmentClass = APTEquipment::StaticClass();
	PTEngineerPropClass = APTEngineerProp::StaticClass();

	if (World)
	{
		PTFireArm = World->SpawnActor<APTFireArm>(PTFireArmClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
		PTWeapon = World->SpawnActor<APTWeapon>(PTWeaponClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
		PTEquipment = World->SpawnActor<APTEquipment>(PTEquipmentClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);
		PTEngineerProp = World->SpawnActor<APTEngineerProp>(PTEngineerPropClass, GetActorLocation(), GetActorRotation(), ActorSpawnParams);


		PTFireArm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("PTFireArm")));
		PTWeapon->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("PTWeapon")));
		PTEquipment->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("PTEquipment"))); 
		PTEngineerProp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("PTEngineerProp")));
	}

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	BodyMesh->SetRelativeLocation(FVector(-49.0f, -11.0f, -105.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	ThirdPersonSkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonSkeletalMeshWeapon"));
	ThirdPersonSkeletalMeshWeapon->SetOwnerNoSee(true);
	ThirdPersonSkeletalMeshWeapon->SetupAttachment(BodyMesh);
	ThirdPersonSkeletalMeshWeapon->bCastDynamicShadow = false;
	ThirdPersonSkeletalMeshWeapon->CastShadow = false;

	ThirdPersonStaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThirdPersonStaticMeshWeapon"));
	ThirdPersonStaticMeshWeapon->SetOwnerNoSee(true);
	ThirdPersonStaticMeshWeapon->SetupAttachment(BodyMesh);
	ThirdPersonStaticMeshWeapon->bCastDynamicShadow = false;
	ThirdPersonStaticMeshWeapon->CastShadow = false;


	StaticMeshWeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshWeaponComponent"));
	StaticMeshWeaponComponent->SetOnlyOwnerSee(true);
	StaticMeshWeaponComponent->bCastDynamicShadow = false;
	StaticMeshWeaponComponent->CastShadow = false;
	StaticMeshWeaponComponent->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(Mesh1P);

	TP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TP_MuzzleLocation"));
	TP_MuzzleLocation->SetupAttachment(ThirdPersonSkeletalMeshWeapon);

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	CensoreCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CensoreCapsuleComponent"));
	CensoreCapsuleComponent->SetCapsuleHalfHeight(120);
	CensoreCapsuleComponent->SetCapsuleRadius(50);
	CensoreCapsuleComponent->SetRelativeLocationAndRotation(FVector(0.0f, 20.0f, 90.0f), FRotator(0, 0, 0));
	CensoreCapsuleComponent->SetupAttachment(RootComponent);

	CensoreCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &APrototypeCharacter::OnOverlapBegin);
	CensoreCapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &APrototypeCharacter::OnOverlapEnd);

	WallCounter = 0;

	HP = 100.0f;
	Stamina = 100.0f;
	Armor = 100.0f;

	bPrssedSurf = false;
	bPressedRun = false;
	bPressedJumping = false;

	IsFiring = false;

	PlayerWalkSpeed = 300.0f;
	PlayerRunSpeed = 600.0f;

	StaminaRecovery = 6.0f;

	SoldierPassive = 400.0f;
	
	ArmorRecovery = 0.01f;
	DefenderArmorPassive = 0.02f;

	DoctorPassive = 0.0f;

	this->Tags.Add(TEXT("Player"));
	//this->Tags.Add(TEXT("Default"));
	CurrentPlayerState = EPlayerState::Alive;

	ActiveSkillCooldown = 0.0f;
	DecelerationCount = 3;

	CurrentDamage = 0.0f;
	
	IsUsingDefenderSkill = false;
	IsUsingActiveSkill = false;
	IsReloading = false;

	bReplicates = true;
	CurrentWeaponState = EWeaponState::KnockbackGun;

	MuzzleLocation = FVector::ZeroVector;
	MuzzleRotation = FRotator::ZeroRotator;

	DoctorHeal = 10.0f;
	IsEnterdAccelerationField = false;
}

//벽타기
void APrototypeCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OnGround())
	{
		WallCounter = 1;
		JumpCurrentCount = 0;
	}
}
//벽타기 해제
void APrototypeCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	WallCounter = 0;
	JumpMaxCount = 2;
	JumpCurrentCount = JumpMaxCount;
}

void APrototypeCharacter::IsJump()
{
	if (GetPlayerState() == EPlayerState::Alive)
	{
		if (!OnGround())
		{
			if (WallCounter == 0) //Not wall riding jump
			{
				if (JumpCurrentCount == 1 && Stamina >= 20.1f) // Double jump, need cost
				{	
				//	Stamina = Stamina - 20.0f;
					bPressedJumping = true;
					Jump();
					ReplicateJumpStamina();
				}
			}
			else if (WallCounter != 0)
			{
				if (Stamina >= 20.1f) // If not enough energy, can't wall riding jump  
				{
					JumpMaxCount += 1;
				//	Stamina = Stamina - 20.0f;
					bPressedJumping = true;
					Jump();
					ReplicateJumpStamina();
				}
			}

		}
		else
		{
			bPressedJumping = true;
			Jump();
		}
	}
}

void APrototypeCharacter::Surf()
{
	if (GetPlayerState() == EPlayerState::Alive)
	{
		FHitResult Hit;
		FVector LineTraceBegin = GetActorLocation() + FVector(30.0f, 0, -100.0f);
		FVector LineTraceEnd = LineTraceBegin + GetActorUpVector() * -300.0f;
		FCollisionQueryParams CollisionParams(FName(""), false, GetOwner());

		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

		DrawDebugLine(
			GetWorld(),
			LineTraceBegin,
			LineTraceEnd,
			FColor(255, 0, 0),
			false,
			1.0f,
			0.0f,
			1.0f
		);

		bool isHit = GetWorld()->LineTraceSingleByObjectType(Hit, LineTraceBegin, LineTraceEnd, CollisionObjectQueryParams , CollisionParams);
		if (isHit)
		{
			Hit.Actor->GetActorRotation();

			if (Hit.Actor->GetActorRotation().Roll > 45.0f || Hit.Actor->GetActorRotation().Roll < -45.0f)
			{
				bPrssedSurf = true;
			//	this->GetCharacterMovement()->SetWalkableFloorAngle(90.0f);
				SetSurf(PlayerRunSpeed, true);

			}
		}
	}

}

void APrototypeCharacter::StopSurf()
{
	bPrssedSurf = false;
	this->GetCharacterMovement()->SetWalkableFloorAngle(45.0f);
	SetSurf(PlayerWalkSpeed, false);
	
}

void APrototypeCharacter::Running()
{
	if (GetPlayerState() == EPlayerState::Alive)
	{
		bPressedRun = true;
		SetWalkSpeed(PlayerRunSpeed, true);
	}

}

void APrototypeCharacter::StopRunning()
{
	bPressedRun = false;	
	SetWalkSpeed(PlayerWalkSpeed, false);
}


bool APrototypeCharacter::OnGround()
{
	bool IsGround = GetCharacterMovement()->IsMovingOnGround();

	if (IsGround == true)
	{
		JumpCurrentCount = 0;
		bPressedJumping = false;

	}
	return IsGround;
}

void APrototypeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentPlayerState == EPlayerState::Alive)
	{
		if (CurrentWeaponState == EWeaponState::KnockbackGun)
		{
			if (FP_Gun != nullptr)
			{
				MuzzleLocation = FP_Gun->GetSocketLocation(FName("Muzzle"));
				MuzzleRotation = FP_Gun->GetSocketRotation(FName("Muzzle"));
			}
		}

		if (PTWeapon != nullptr)
		{
			CurrentWeaponState = PTWeapon->GetWeaponState();
		}
		if ((bPrssedSurf == false && bPressedRun == false))
		{
			OnGround();
			if (!bPressedJumping)
			{
				Stamina = Stamina + (StaminaRecovery * DeltaTime);
				if (Stamina < 0)
				{
					Stamina = 0;
				}
				if (Stamina > 100.0f)
				{
					Stamina = 100.0f;
				}
			}
		}
		if (HP < 100.0f && this->ActorHasTag(TEXT("Doctor")))
		{
			HP = HP + (DoctorPassive * DeltaTime);
			if (HP > 100.0f)
			{
				HP = 100.0f;
			}
		}
		//Armor 자동 회복
		if (Armor < 100.0f && HP >= 1.0f)
		{
			Armor = Armor + ArmorRecovery;

			if (Armor > 100.0f)
			{
				Armor = 100.0f;
			}
		}
		//Run 상태일 경우 최대 속도 향상
		if (bPressedRun || bPrssedSurf )
		{
			this->GetCharacterMovement()->MaxWalkSpeed = PlayerRunSpeed;
		}
		//Run 상태가 아닐 경우 최대 속도 정상화
		else if (bPressedRun == false && bPrssedSurf == false)
		{
			this->GetCharacterMovement()->MaxWalkSpeed = PlayerWalkSpeed;
		}

		//Surf or Running 상태일 경우 Stanmina 지속 감소 : 초당 5
		if ((bPrssedSurf || bPressedRun) && Stamina > 1.0f && Stamina > 0.0f)
		{
			if (bPrssedSurf)
			{
				this->GetCharacterMovement()->SetWalkableFloorAngle(90.0f);
			}

			Stamina = Stamina - (5.0f * DeltaTime);
		}
		//Stamina가 부족할 경우 Surf 중지
		else if (bPrssedSurf && Stamina < 1.0f)
		{
			StopSurf();
		}
		else if (bPressedRun && Stamina < 1.0f)
		{
			StopRunning();
		}
		//Soldier를 제외한 다른 클래스 ActiveSkill 프로그레스바 활성화
		if (!(this->ActorHasTag(TEXT("Soldier"))))
		{
			if (ActiveSkillCooldown < 100.0f)
			{
				ActiveSkillCooldown = ActiveSkillCooldown + (7.0f * DeltaTime);
			}
		}

	}
	//살아있는 상태가 아니면, 무빙 정지
	else if(GetPlayerState() == EPlayerState::Groggy || GetPlayerState() == EPlayerState::Dead)
	{
		this->GetCharacterMovement()->StopMovementImmediately();
	}
	//중력 상태일 경우 속도 저하
	else if (GetPlayerState() == EPlayerState::InMaxGravity)
	{
		this->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
	}
}

void APrototypeCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Mesh1P->SetHiddenInGame(false, true);

	BodyMeshAnimInstance = Cast<UPTAnimInstance>(GetBodyMesh()->GetAnimInstance());

}

//Engineer의 가속 필드를 지날 경우 가속
void APrototypeCharacter::Acceleration()
{
	PlayerWalkSpeed = 1200.0f;
	PlayerRunSpeed = 1200.0f;
	DecelerationCount = 3;
	IsEnterdAccelerationField = true;
	Deceleration();
}

//가속 후 3초에 걸쳐서 감속 -> 속도 정상화
void APrototypeCharacter::Deceleration()
{
	PlayerWalkSpeed = PlayerWalkSpeed - 300.0f;
	PlayerRunSpeed = PlayerRunSpeed - 200.0f;

	DecelerationCount--;

	if (DecelerationCount >= 0)
	{
		GetWorld()->GetTimerManager().SetTimer(timer, this, &APrototypeCharacter::Deceleration, 1.0f, false);
	}
	else
	{
		IsEnterdAccelerationField = false;
		PlayerWalkSpeed = 300.0f;
		PlayerRunSpeed = 600.0f;
	}
}

void APrototypeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
		PlayerInputComponent->BindAction("VirtualDamage", IE_Pressed, this, &APrototypeCharacter::ArmorChange);

		//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APrototypeCharacter::IsJump);
	
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

		// Bind fire event
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APrototypeCharacter::StartFire);
		PlayerInputComponent->BindAction("Fire", IE_Released, this, &APrototypeCharacter::StopFire);
		PlayerInputComponent->BindAction("Reloading", IE_Pressed, this, &APrototypeCharacter::Reloading);

		PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &APrototypeCharacter::ActiveSkill);

		PlayerInputComponent->BindAction("KnockbackGun", IE_Pressed, this, &APrototypeCharacter::ChangeKnockbackGun);
		PlayerInputComponent->BindAction("ClassWeapon", IE_Pressed, this, &APrototypeCharacter::ChangeClassWeapon);
	
		PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APrototypeCharacter::Running);
		PlayerInputComponent->BindAction("Run", IE_Released, this, &APrototypeCharacter::StopRunning);

		PlayerInputComponent->BindAction("Surf", IE_Pressed, this, &APrototypeCharacter::Surf);
		PlayerInputComponent->BindAction("Surf", IE_Released, this, &APrototypeCharacter::StopSurf);

		// Bind movement events
		PlayerInputComponent->BindAxis("MoveForward", this, &APrototypeCharacter::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &APrototypeCharacter::MoveRight);
	
		PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
		PlayerInputComponent->BindAxis("TurnRate", this, &APrototypeCharacter::TurnAtRate);
		PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("LookUpRate", this, &APrototypeCharacter::LookUpAtRate);
}

//Armor가 Damage 보다 수치가 적을 경우 HP 감소로 넘겨 줌
void APrototypeCharacter::ArmorChange()
{
	if (HP >= 0.0f)
	{
		if (Armor >= 0.0f)
		{
			if (CurrentDamage == 0.0f)
			{
				CurrentDamage = 40.0f;
			}
			float FinalDamage = CurrentDamage - Armor;
			Armor = Armor - CurrentDamage;
			if (Armor <= 0.0f)
			{
				Armor = 0.0f;
				HPChange(FinalDamage);
			}
		}
	}
	else
		return;
}

//Defender의 PassiveSkill 영역 안에 있을 경우 Armor 회복
void APrototypeCharacter::SetArmor(float NewArmor, bool bUsing)
{
	Armor = NewArmor;
	IsUsingDefenderSkill = bUsing;
}

//HP Damage 입어 감소
void APrototypeCharacter::HPChange(float Damage)
{
	if (HP > 0.0f)
	{
		HP = HP - Damage;
		if (HP < 0.0f)
		{
			HP = 0.0f;
			if (HP <= 0.0f)
			{
				if (CurrentPlayerState == EPlayerState::Groggy)
				{
					SetDead();
				}
				else if (CurrentPlayerState == EPlayerState::Alive)
				{
					SetGroggy();
				}

			}
		}
	}
}

//ActiveSkiil
void APrototypeCharacter::ActiveSkill()
{
	UWorld* World = GetWorld();
	
	if (CurrentPlayerState == EPlayerState::Alive && !bPressedRun)
	{
		if (ActiveSkillCooldown >= 100.0f)
		{
			if (this->ActorHasTag(TEXT("Defender")))
			{
				if (CurrentWeaponState == EWeaponState::DefenderWeapon)
				{
					PTEquipment->SetActiveSkill(this->GetActorLocation(), this->GetActorRotation(), World, CurrentWeaponState, this);
					PlaySoundParticle();
					PTFireArm->IsFireSuccess = false;
				}
			}

			else if (this->ActorHasTag(TEXT("Doctor")))
			{
				if (CurrentWeaponState == EWeaponState::DoctorWeapon)
				{
					PTEquipment->SetActiveSkill(this->GetActorLocation() + FirstPersonCameraComponent->GetForwardVector(), this->GetActorRotation(), World, CurrentWeaponState, this);
					PlaySoundParticle();
					PTEquipment->IsFireSuccess = false;			
				}
			}

			else if (this->ActorHasTag(TEXT("Engineer")))
			{
				if (CurrentWeaponState == EWeaponState::EngineerWeapon)
				{
					PTEngineerProp->SetActiveSkill(CurrentWeaponState, this);
				}
			}

			ActiveSkillCooldown = 0.0f;
			ReplicateActiveCooldown();
		}

		if (this->ActorHasTag(TEXT("Soldier")))
		{
			if (CurrentWeaponState == EWeaponState::SoldierWeapon)
			{
				if (IsReloading == false)
				{
					PTFireArm->SetActiveSkill(CurrentWeaponState, this);
					IsUsingActiveSkill = true;
					PlaySoundParticle();
					IsUsingActiveSkill = false;
				}
			}
		}
	}

}

//클래스별 Passive Setting
void APrototypeCharacter::SetPlayerPassive()
{
	if (this->ActorHasTag(TEXT("Soldier")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor has Soldier tag"));
		PlayerWalkSpeed = SoldierPassive;
		ActiveSkillCooldown = 0.0f;
	}
	else if (this->ActorHasTag(TEXT("Defender")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor has Defender tag"));
		ArmorRecovery = DefenderArmorPassive;
		HP = HP + 50.0f;
	}
	else if (this->ActorHasTag(TEXT("Doctor")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor has Doctor tag"));
		DoctorPassive = 0.5f;
	}
	else if (this->ActorHasTag(TEXT("Engineer")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor has Engineer tag"));
		PTFireArm->MaxKnockbackAmmo = 50.0f;
	}

	SetPlayerWeaponMeshSocket();
}

void APrototypeCharacter::SetPlayerWeaponMeshSocket()
{
	UE_LOG(LogTemp, Warning, TEXT("Set Player Weapon Mesh Socket"));
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	ThirdPersonSkeletalMeshWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
	ThirdPersonSkeletalMeshWeapon->SetRelativeLocation(FVector(0.0f, 13.0f, 0.0f));
	ThirdPersonSkeletalMeshWeapon->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));

	FP_MuzzleLocation->SetRelativeLocationAndRotation(FVector(100.0f, 25.0f, 140.0f), FRotator(-5.2f, -1.9f, 19.0f));

	//Defender는 장비가 개별 소켓에 위치
	if (this->ActorHasTag(TEXT("Defender")))
	{
		StaticMeshWeaponComponent->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("DefenderSocket"));
		ThirdPersonStaticMeshWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("DefenderSocket"));
	}
	else if (this->ActorHasTag(TEXT("Engineer")))
	{
		StaticMeshWeaponComponent->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("EngineerSocket"));
		ThirdPersonStaticMeshWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("EngineerSocket"));
	}
	else
	{
		StaticMeshWeaponComponent->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		ThirdPersonStaticMeshWeapon->AttachToComponent(BodyMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSocket"));
	}

}


//KnockbackGun으로 교체
void APrototypeCharacter::ChangeKnockbackGun()
{
	if (!IsReloading)
	{
		if (CurrentPlayerState == EPlayerState::Alive)
		{
			PTWeapon->ChangeKnockbackGun(this);
			
		}
	}
	else
		return;
}

//ClassWeapon으로 교체
void APrototypeCharacter::ChangeClassWeapon()
{
	if (!IsReloading)
	{
		if (CurrentPlayerState == EPlayerState::Alive)
		{
			PTWeapon->ChangeClassWeapon(this);
		}
		else
			return;
	}
}

//Soldier 연발 상태 시작
void APrototypeCharacter::StartFire()
{
	IsFiring = true;
	OnFire();
}

//Soldier 연발 취소
void APrototypeCharacter::StopFire()
{
	IsFiring = false;
}

void APrototypeCharacter::OnFire()
{
	if (CurrentPlayerState == EPlayerState::Alive && !IsReloading && !bPressedRun)
	{
		if (IsFiring)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				const FRotator SpawnRotation = GetControlRotation();
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
				
				//Soldier나 knockback은 muzzle location 반환
				if (CurrentWeaponState == EWeaponState::KnockbackGun || CurrentWeaponState == EWeaponState::SoldierWeapon)
				{
					PTFireArm->Fire(CurrentWeaponState, this);
					PlaySoundParticle();
				
				}
				//Defender or Doctor는 카메라의 위치를 반환
				else if (CurrentWeaponState == EWeaponState::DoctorWeapon)
				{
					PTEquipment->Fire(CurrentWeaponState, this);
					PlaySoundParticle();
				
				}
				else if (CurrentWeaponState == EWeaponState::DefenderWeapon)
				{
					PTEquipment->Fire(CurrentWeaponState, this);
					PlaySoundParticle();

				}
				else if (CurrentWeaponState == EWeaponState::EngineerWeapon)
				{
					PTEngineerProp->SetPassiveSkill(this->GetActorLocation(), this->GetActorForwardVector(), World, CurrentWeaponState);
				}

			}
			//발사 모드가 Auto일 경우 연사 가능 0.1초당 1발 씩
			if (PTFireArm->GetFireMode() == EFireMode::Auto)
			{
				GetWorld()->GetTimerManager().SetTimer(timer, this, &APrototypeCharacter::OnFire, 0.1f, false);
			}
			else
			{
				return;
			}
			
		}
	}
}

//사운드 및 파티클 재생
void APrototypeCharacter::PlaySoundParticle()
{
	if (CurrentWeaponState == EWeaponState::KnockbackGun)
	{
		if (PTFireArm->IsFireSuccess == true)
		{
			if (KnockbackFireSound != NULL && KnockbackFireParticle != NULL)
			{
				SetPlayFPPSoundAndParticle(KnockbackFireParticle, KnockbackFireSound);

				APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
				if (PlayerController != nullptr)
				{
					PlayerController->PlaySoundAndParticle(this, KnockbackFireParticle, KnockbackFireSound);
				}
				PlayAnimation();
			}
		}
	}

	else if (CurrentWeaponState == EWeaponState::SoldierWeapon)
	{
		if (PTFireArm->IsFireSuccess == true)
		{
			//액티브 스킬이 아닌 경우 (연발 소총)
			if (SoldierPassiveFireSound != NULL && SoldierPassiveFireParticle != NULL && IsUsingActiveSkill == false)
			{
				SetPlayFPPSoundAndParticle(SoldierPassiveFireParticle, SoldierPassiveFireSound);

				APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
				if (PlayerController != nullptr)
				{
					PlayerController->PlaySoundAndParticle(this, SoldierPassiveFireParticle, SoldierPassiveFireSound);
				}
			}

			//액티브 스킬인 경우 (유탄 발사) , 솔져 액티브는 파티클 없음
			else if (SoldierActiveFireSound != NULL  && IsUsingActiveSkill == true)
			{
				SetPlayFPPSoundAndParticle(nullptr, SoldierActiveFireSound);

				APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
				if (PlayerController != nullptr)
				{
					PlayerController->PlaySoundAndParticle(this, nullptr, SoldierActiveFireSound);
				}
			}
		
			PlayAnimation();
		}
	}

	else if (CurrentWeaponState == EWeaponState::DoctorWeapon)
	{
		if (PTEquipment->IsFireSuccess == true)
		{
			if (DoctorPassiveFireSound != NULL)
			{
				SetPlayFPPSoundAndParticle(nullptr, DoctorPassiveFireSound);

				APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
				if (PlayerController != nullptr)
				{
					PlayerController->PlaySoundAndParticle(this, nullptr, DoctorPassiveFireSound);
				}
			}

			PlayAnimation();
		}
	}

}

//1인칭 모습 파티클 재생
void APrototypeCharacter::SetPlayFPPSoundAndParticle(UParticleSystem* Particle, USoundBase* Sound)
{
	if (Sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}

	if (Particle != nullptr)
	{
		if (FP_MuzzleLocation != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(Particle, FP_MuzzleLocation, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		}
	}
}

void APrototypeCharacter::SetPlayTPPSoundAndParticle(UParticleSystem* Particle, USoundBase* Sound)
{
	if (this != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());

		if (Particle != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(Particle, GetThirdPersonMuzzleLocation(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);		
		}
	}
}

//애니메이션 재생 : 해당 애니메이션 인스턴스를 받아서 인스턴스에서 몽타주 실행
void APrototypeCharacter::PlayAnimation()
{
	if (CurrentWeaponState == EWeaponState::KnockbackGun || CurrentWeaponState == EWeaponState::DoctorWeapon || CurrentWeaponState == EWeaponState::SoldierWeapon)
	{
		auto AnimInstance = Cast<UPTAnimInstance>(GetMesh1P()->GetAnimInstance());
		if (AnimInstance != NULL)
		{
			AnimInstance->PlayFIreMontage();
		}

		auto BodyAnimInstance = Cast<UPTAnimInstance>(GetBodyMesh()->GetAnimInstance());

		if (BodyAnimInstance != NULL)
		{	
			APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
			PlayerController->PlayFireMontage(this);
		
		}
		
	}
}

//재장전
void APrototypeCharacter::Reloading()
{
	if (GetPlayerState() == EPlayerState::Alive)
	{
		if (!BodyMeshAnimInstance->Montage_IsPlaying(BodyMeshAnimInstance->TPPReloadMontage))
		{
			IsReloading = true;
		}
		else
		{
			return;
		}
	

		int NeedAmmo = 0;
		FTimerHandle WaitHandle;
		float WaitTime = 1.0f; //재장전 시간 설정

		if (CurrentWeaponState == EWeaponState::KnockbackGun)
		{
			if (PTFireArm->MaxKnockbackAmmo == 0 && PTFireArm->CurrentKnockbackAmmo < 10)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
			if (PTFireArm->CurrentKnockbackAmmo == 10)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
		}

		else if (CurrentWeaponState == EWeaponState::SoldierWeapon)
		{
			if (PTFireArm->MaxKSoldierAmmo == 0 && PTFireArm->CurrentSoldierAmmo < 30)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
			if (PTFireArm->CurrentSoldierAmmo == 30)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
		}

		else if (CurrentWeaponState == EWeaponState::DefenderWeapon)
		{
			if (PTEquipment->MaxDefenderShieldAmmo == 0 && PTEquipment->CurrentDefenderShieldAmmo < 1)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
			if (PTEquipment->CurrentDefenderShieldAmmo == 1)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
		}

		else if (CurrentWeaponState == EWeaponState::DoctorWeapon)
		{
			if (PTEquipment->MaxDoctorPassiveAmmo == 0 && PTEquipment->CurrentDoctorPassiveAmmo < 10)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
			if (PTEquipment->CurrentDoctorPassiveAmmo == 10)
			{
				IsReloading = false;
				WaitTime = 0.0f;
				return;
			}
		}
		//재장전 가능할 경우 재장전 모션 실행
 		auto AnimInstance = Cast<UPTAnimInstance>(GetMesh1P()->GetAnimInstance());
		if (AnimInstance != NULL)
		{
			AnimInstance->PlayReloadMontage();
		}

		APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
		if (PlayerController != nullptr)
		{
			PlayerController->PlayReloadMontage(BodyMeshAnimInstance, this);
		}
	
		if (ReloadingSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReloadingSound, GetActorLocation());
		}


		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			IsReloading = false;

			// 코드 입력
			if (PTWeapon->GetWeaponState() == EWeaponState::KnockbackGun)
			{
				if (PTFireArm->MaxKnockbackAmmo > 0)
				{
					NeedAmmo = 10 - PTFireArm->CurrentKnockbackAmmo;
				
					if (PTFireArm->MaxKnockbackAmmo >= NeedAmmo)
					{
						PTFireArm->MaxKnockbackAmmo = PTFireArm->MaxKnockbackAmmo - NeedAmmo;
						PTFireArm->CurrentKnockbackAmmo = PTFireArm->CurrentKnockbackAmmo + NeedAmmo;
					}

					else if (PTFireArm->MaxKnockbackAmmo < NeedAmmo)
					{
						PTFireArm->CurrentKnockbackAmmo = PTFireArm->MaxKnockbackAmmo;
						PTFireArm->MaxKnockbackAmmo = 0;
					}

				}
				else
					return;
			}

			else if (PTWeapon->GetWeaponState() == EWeaponState::SoldierWeapon)
			{
				if (PTFireArm->MaxKSoldierAmmo > 0)
				{
					NeedAmmo = 30 - PTFireArm->CurrentSoldierAmmo;

					if (PTFireArm->MaxKSoldierAmmo >= NeedAmmo)
					{
						PTFireArm->MaxKSoldierAmmo = PTFireArm->MaxKSoldierAmmo - NeedAmmo;
						PTFireArm->CurrentSoldierAmmo = PTFireArm->CurrentSoldierAmmo + NeedAmmo;
					}

					else if (PTFireArm->MaxKSoldierAmmo < NeedAmmo)
					{
						PTFireArm->CurrentSoldierAmmo = PTFireArm->MaxKSoldierAmmo;
						PTFireArm->MaxKSoldierAmmo = 0;
					}

				}
				else
					return;
			}

			else if (PTWeapon->GetWeaponState() == EWeaponState::DefenderWeapon)
			{
				if (PTEquipment->MaxDefenderShieldAmmo != 0)
				{
					if (PTEquipment->CurrentDefenderShieldAmmo != 1)
					{
						PTEquipment->MaxDefenderShieldAmmo--;
						PTEquipment->CurrentDefenderShieldAmmo++;
					}
					else
					{
						return;
					}
			
				}
			}

			else if (PTWeapon->GetWeaponState() == EWeaponState::DoctorWeapon)
			{
				if (PTEquipment->MaxDoctorPassiveAmmo > 0)
				{
					NeedAmmo = 10 - PTEquipment->CurrentDoctorPassiveAmmo;

					if (PTEquipment->MaxDoctorPassiveAmmo >= NeedAmmo)
					{
						PTEquipment->MaxDoctorPassiveAmmo = PTEquipment->MaxDoctorPassiveAmmo - NeedAmmo;
						PTEquipment->CurrentDoctorPassiveAmmo = PTEquipment->CurrentDoctorPassiveAmmo + NeedAmmo;
					}

					else if (PTEquipment->MaxDoctorPassiveAmmo < NeedAmmo)
					{
						PTEquipment->CurrentDoctorPassiveAmmo = PTEquipment->MaxDoctorPassiveAmmo;
						PTEquipment->MaxDoctorPassiveAmmo = 0;
					}

				}
				else
					return;
			}

		}), WaitTime, false);
	
	}
}

//닥터에 의해 소생 시, 체력 50으로 회복 디펜더는 75
void APrototypeCharacter::SetRevive()
{
	if (CurrentPlayerState ==EPlayerState::Groggy)
	{
		if (this->ActorHasTag(TEXT("Defender")))
		{
			this->HP = 75.0f;
		}
		else
		{
			this->HP = 50.0f;
		}
		this->Armor = 50.0f;
		this->Stamina = 50.0f;

		CurrentPlayerState = EPlayerState::Alive;
	}

}

// 살아있는 상태에서 체력이 0 일시 그로기 상태 돌입 체력 50으로 정지상태
void APrototypeCharacter::SetGroggy()
{
	CurrentPlayerState = EPlayerState::Groggy;
	this->HP = 50.0f;
	this->Armor = 0.0f;
	this->Stamina = 0.0f;
}

//그로기에서 체력이 0일 경우 사망
void APrototypeCharacter::SetDead()
{
	CurrentPlayerState = EPlayerState::Dead;
	this->HP = 0.0f;
	this->Armor = 0.0f;
	this->Stamina = 0.0f;
	this->SetCanBeDamaged(false);

	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set Sepectator"));
		PlayerController->ChangeSpectatorMode();
	}
}

//플레이어 데미지 받을 경우 처리
float APrototypeCharacter::TakeDamage(float DamageAmount,FDamageEvent const &DamageEvent, AController* EventInstigator,AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("In PTC Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	CurrentDamage = FinalDamage;
	ArmorChange();
	if (HP <= 0.0f)
	{
		if (CurrentPlayerState == EPlayerState::Groggy)
		{
			SetDead();
		}
		else if(CurrentPlayerState == EPlayerState::Alive || CurrentPlayerState == EPlayerState::InMaxGravity || CurrentPlayerState == EPlayerState::InZeroGravity)
		{
			SetGroggy();
		}
		
	}
	return FinalDamage;
}

void APrototypeCharacter::MoveForward(float Value)
{
	if (CurrentPlayerState == EPlayerState::Alive || CurrentPlayerState == EPlayerState::InMaxGravity || CurrentPlayerState == EPlayerState::InZeroGravity)
	{
		if (bPrssedSurf == true && Value != 0.0f)
		{
			FVector Direction = FirstPersonCameraComponent->GetForwardVector();
			AddMovementInput(Direction, Value);
		}
	
		else if (Value != 0.0f && bPrssedSurf == false)
		{
			// add movement in that direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}
}

void APrototypeCharacter::MoveRight(float Value)
{
	if (CurrentPlayerState == EPlayerState::Alive || CurrentPlayerState == EPlayerState::InMaxGravity || CurrentPlayerState == EPlayerState::InZeroGravity)
	{
		if (bPrssedSurf == true && Value != 0.0f)
		{
			AddMovementInput(GetActorRightVector(), Value);
		}

		else if (bPrssedSurf == false && Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void APrototypeCharacter::TurnAtRate(float Rate)
{
	if (bPrssedSurf == false)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void APrototypeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APrototypeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APrototypeCharacter, HP);
	DOREPLIFETIME(APrototypeCharacter, Armor);
	DOREPLIFETIME(APrototypeCharacter, Stamina);
	DOREPLIFETIME(APrototypeCharacter, bPrssedSurf);
	DOREPLIFETIME(APrototypeCharacter, bPressedRun);
	DOREPLIFETIME(APrototypeCharacter, bPressedJumping);
	DOREPLIFETIME(APrototypeCharacter, IsEnterdAccelerationField);
	DOREPLIFETIME(APrototypeCharacter, FP_Gun);
	DOREPLIFETIME(APrototypeCharacter, SoldierPassive);
	DOREPLIFETIME(APrototypeCharacter, PlayerWalkSpeed);
	DOREPLIFETIME(APrototypeCharacter, PlayerRunSpeed);
	DOREPLIFETIME(APrototypeCharacter, ArmorRecovery);
	DOREPLIFETIME(APrototypeCharacter, DefenderArmorPassive);
	DOREPLIFETIME(APrototypeCharacter, DoctorPassive);
	DOREPLIFETIME(APrototypeCharacter, CurrentPlayerState);
	DOREPLIFETIME(APrototypeCharacter, CurrentDamage);
	DOREPLIFETIME(APrototypeCharacter, IsReloading);
	DOREPLIFETIME(APrototypeCharacter, IsFiring);
	DOREPLIFETIME(APrototypeCharacter, DecelerationCount);
	DOREPLIFETIME(APrototypeCharacter, CurrentWeaponState);
	DOREPLIFETIME(APrototypeCharacter, MuzzleLocation);
	DOREPLIFETIME(APrototypeCharacter, MuzzleRotation);
	DOREPLIFETIME(APrototypeCharacter, DoctorHeal);
	DOREPLIFETIME(APrototypeCharacter, StaminaRecovery);
	DOREPLIFETIME(APrototypeCharacter, ActiveSkillCooldown);


}

void APrototypeCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

//피격 당한 NPC HP 변경 -> 플레이어 컨트롤러로 전달 후 처리
void APrototypeCharacter::SetNPCHP_Implementation(AActor* TargetActor, float Damage)
{

	if (ROLE_Authority == GetLocalRole())
	{
		if (TargetActor->ActorHasTag(TEXT("NPC")))
		{
			
			APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());

			if (PlayerController != nullptr)
			{				
				PlayerController->SetNPCHP(TargetActor, Damage);
			}

		}

	}

}

//플레이어 점프 스테미너 동기화 ->플레이어 컨트롤러로 전달 후 처리
void APrototypeCharacter::ReplicateJumpStamina()
{
	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());

	if (PlayerController != nullptr)
	{
		PlayerController->ReplicateJumpStamina(this, bPressedJumping);
		
	}
}

void APrototypeCharacter::ReplicateActiveCooldown()
{
	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());

	if (PlayerController != nullptr)
	{
		PlayerController->ReplicateActiveCooldown(this);
	}
}

//플레이어 이동 속도 동기화 ->플레이어 컨트롤러로 전달 후 처리
void APrototypeCharacter::SetWalkSpeed_Implementation(float Speed , bool RunState)
{
	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());

	if (ROLE_Authority == GetLocalRole())
	{
		if (PlayerController != nullptr)
		{
			PlayerController->ReplicateCharacterWalkSpeed(this, Speed , RunState);
			//UE_LOG(LogTemp, Warning, TEXT("Send RPC to PlayerController 1"));
		}
		
	}
}

//서프시 이동 속도 동기화 -> 호출 -> 플레이어 컨트롤러로 전달 후 처리
void APrototypeCharacter::SetSurf_Implementation(float Speed, bool SurfState)
{
	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());

	if (ROLE_Authority == GetLocalRole())
	{
		if (PlayerController != nullptr)
		{
			PlayerController->ReplicateSurf(this, Speed, SurfState);
			//UE_LOG(LogTemp, Warning, TEXT("Send RPC to PlayerController 1"));
		}

	}
}

//터렛 Hacking -> 호출 -> 플레이어 컨트롤러로 전달 후 처리
void APrototypeCharacter::SetTurretHacking_Implementation(class APTTurret* Turret)
{
	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
	if (Turret != nullptr)
	{
		PlayerController->TurretHacking(Turret);
	}
}

//터렛 DetectedTarget -> 호출 -> 플레이어 컨트롤러로 전달 후 처리
void APrototypeCharacter::SetTurretFire_Implementation(class APTTurret* Turret, AActor* TargetActor)
{
	APTPlayerController* PlayerController = Cast<APTPlayerController>(GetController());
	if (Turret != nullptr)
	{
		if (TargetActor != nullptr)
		{
			PlayerController->TurretFire(Turret, TargetActor, GetWorld());
		}
	}
}

//터렛 Fire로 부터 호출
void APrototypeCharacter::SetTurretParticle_Implementation(class APTTurret* Turret)
{
	if (Turret != nullptr)
	{
		Turret->PlayParticleAndSound();
	}
}

/**
* @author : 황현준
* @date : 2021-04-29
* @description : 
* @updater : 황현준
* @update : 리플리케이션 구현 방식을 PlayerController에서 -> PlayerState에서 실행되도록 변경.
* @param : 
*/
void APrototypeCharacter::SetAcceleration_Implementation()
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(APawn::GetPlayerState());

	if (PTPlayerState != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set Acceleration On Server"));
		PTPlayerState->SetAcceleration(this);
	}
}

//필요없음 추후 삭제 요망
void APrototypeCharacter::SetDeceleration_Implementation()
{

}

//이큅먼트 SetActiveSkiil -> 호출 -> 플레이어스테이트로 전달 후 처리
void APrototypeCharacter::SetDoctorActive_Implementation(FVector PlayerLocation, FRotator PlayerRotation, UWorld* World, APTEquipment* Equipment)
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(APawn::GetPlayerState());

	if (PTPlayerState != nullptr)
	{
		PTPlayerState->DoctorActive(GetActorLocation(), GetActorRotation(), GetWorld());
	}
	
}

/**
* @author : 황현준
* @date : 2021-04-29
* @description :
* @updater : 황현준
* @update : 리플리케이션 구현 방식을 PlayerController에서 -> PlayerState에서 실행되도록 변경.
* @param :
*/
void APrototypeCharacter::SetDefenderPassive_Implementation(APrototypeCharacter* PlayerCharacter, float ShieldValue, bool bUsingDefenderPassiveSkill)
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(APawn::GetPlayerState());
	if (PlayerCharacter != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Defender Passive On Character"));
		if (PlayerCharacter != nullptr)
		{
			PTPlayerState->SetDefenderPassive(this, ShieldValue, bUsingDefenderPassiveSkill);
		}
	}
	
}

//이큅먼트 SetActiveSkiil -> 호출 -> 플레이어스테이트로 전달 후 처리
void APrototypeCharacter::SetDefenderActive_Implementation(FVector PlayerLocation, FRotator PlayerRotation, UWorld* World, APTEquipment* Equipment)
{
	APTPlayerState* PTPlayerState = Cast<APTPlayerState>(APawn::GetPlayerState());

	if (PTPlayerState != nullptr)
	{
		PTPlayerState->DefenderActive(GetActorLocation(), GetActorRotation(), GetWorld(), this);
	}
}

//이큅먼트 DefenderActive -> 호출 후 처리
void APrototypeCharacter::PlayDefenderActiveParticle_Implementation(APrototypeNPCCharacter* TargetActor)
{
	if (TargetActor != nullptr)
	{
		TargetActor->PlayDefenderActiveParticle();
	}
}


/////////////////////////아래 부터는 RPC 액터 스폰////////////////////

//캐릭터 ActiveSkill -> 호출 후 처리
void APrototypeCharacter::SetSpawnSoldierActiveProjectile_Implementation()
{
	APrototypeGameMode* PTGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (PTGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Soldier Active Projectile"));
		PTGameMode->SpawnSoldierActiveProjectile(FP_MuzzleLocation->GetComponentLocation(), GetControlRotation(), GetWorld(), this);
	}

}

//캐릭터 FIre -> 호출 후 처리
void APrototypeCharacter::SetSpawnSoldierPassiveProjectile_Implementation()
{
	
	APrototypeGameMode* PTGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (PTGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Soldier Passive Projectile"));
		PTGameMode->SpawnSoldierPassiveProjectile(FP_MuzzleLocation->GetComponentLocation(), GetControlRotation(), GetWorld(), this);
	}

}

//캐릭터 FIre -> 호출 후 처리
void APrototypeCharacter::SetSpawnKnockbackProjectile_Implementation()
{
	APrototypeGameMode* PTGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (PTGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Knockback Projectile"));
		PTGameMode->SpawnKnockbackProjectile(FP_MuzzleLocation->GetComponentLocation(), GetControlRotation(), GetWorld(), this);
	}
}

//이큅먼트 Fire -> 호출 -> 플레이어컨트롤러로 전달 후 처리
void APrototypeCharacter::SetSpawnDefenderShield_Implementation()
{
	APrototypeGameMode* PTGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (PTGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Denfender Shield"));
		PTGameMode->SpawnDefenderPassive(GetActorLocation(), GetActorRotation(), GetWorld());	
	}
}

//이큅먼트 Fire -> 호출 -> 플레이어컨트롤러로 전달 후 처리
void APrototypeCharacter::SetSpawnDoctorPassiveProjectile_Implementation()
{
	APrototypeGameMode* PTGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (PTGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Doctor Passive"));
		PTGameMode->SpawnDoctorPassiveProjectile(GetActorLocation() + GetActorForwardVector() * 75.0f, GetControlRotation(), GetWorld(), this);
	}
}

//엔지니어프롭 Fire -> 호출 -> 플레이어컨트롤러로 전달 후 처리
void APrototypeCharacter::SetSpawnEngineerActive_Implementation()
{
	APrototypeGameMode* PTGameMode = Cast<APrototypeGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (PTGameMode != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Engineer Active"));
		PTGameMode->SpawnEngineerActive(GetActorLocation() + FirstPersonCameraComponent->GetForwardVector() * 200.0f, GetActorRotation(), GetWorld());
	}
	
}