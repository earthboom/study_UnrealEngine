// Fill out your copyright notice in the Description page of Project Settings.


#include "SPCharacter.h"
#include "SPAnimInstance.h"
#include "SPWeapon.h"
#include "SPCharacterStatComponent.h"
#include "SPCharacterWidget.h"
#include "SPAIController.h"
#include "SPCharacterSetting.h"
#include "SPGameInstance.h"
#include "SPPlayerController.h"
#include "SPPlayerState.h"
#include "SPHUDWidget.h"
#include "SPGameModeBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

// Sets default values
ASPCharacter::ASPCharacter()
	: Super()
	, IsAttacking(false)
	, ArmLengthSpeed(3.0f), ArmRotationSpeed(10.0f)
	, MaxCombo(4), AttackRange(80.0f), AttackRadius(50.0f)
	, AssetIndex(4), DeadTimer(5.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<USPCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBluePrint.WarriorAnimBluePrint_C"));
	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	SetControlMode(EControlMode::DIABLO);

	GetCharacterMovement()->JumpZVelocity = 800.0f;

	AttackEndComboState();

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("SPCharacter"));

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	AIControllerClass = ASPAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//auto DefaultSetting = GetDefault<USPCharacterSetting>();
	//if (DefaultSetting->CharacterAssets.Num() > 0)
	//{
	//	for (auto CharacterAsset : DefaultSetting->CharacterAssets)
	//	{
	//		ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
	//	}
	//}

	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	bCanBeDamaged = false;
}

void ASPCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
	{
		if (bIsPlayer)
		{
			DisableInput(SPPlayerController);

			SPPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto SPPlayerState = Cast<ASPPlayerState>(GetPlayerState());
			ABCHECK(nullptr != SPPlayerState);
			CharacterStat->SetNewLevel(SPPlayerState->GetCharacterLevel());
		}
		else
		{
			auto SPGameMode = Cast<ASPGameModeBase>(GetWorld()->GetAuthGameMode());
			ABCHECK(nullptr != SPGameMode);
			int32 TargetLevel = FMath::CeilToInt(((float)SPGameMode->GetScore() * 0.8f));
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}

		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		bCanBeDamaged = false;
		break;
	}
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		bCanBeDamaged = true;

		CharacterStat->OnHPIsZero.AddLambda([this]()->void {
			SetCharacterState(ECharacterState::DEAD);
		});

		auto CharacterWidget = Cast<USPCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);
		
		if (bIsPlayer)
		{
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(SPPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			SPAIController->RunAI();
		}

		break;
	}
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		SPAnim->SetDeadAnim();
		bCanBeDamaged = false;

		if (bIsPlayer)
			DisableInput(SPPlayerController);
		else
			SPAIController->StopAI();

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void {

			GetWorld()->GetTimerManager().ClearTimer(DeadTimerHandle);

			if (bIsPlayer)
				SPPlayerController->ShowResultUI();
			else
				Destroy();
		}), DeadTimer, false);

		break;
	}
	}
}

ECharacterState ASPCharacter::GetCharacterState() const
{
	return CurrentState;
}

int32 ASPCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float ASPCharacter::GetFinalAttackRange() const
{
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

float ASPCharacter::GetFinalAttackDamage() const
{
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.0f;

	return AttackDamage * AttackModifier;
}

// Called when the game starts or when spawned
void ASPCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Weapon
	//FName WeaponSocket(TEXT("hand_rSocket"));
	//auto CurWeapon = GetWorld()->SpawnActor<ASPWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	//if (CurWeapon != nullptr)
	//	CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);


	//auto CharacterWidget = Cast<USPCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	//if (nullptr != CharacterWidget)
	//{
	//	CharacterWidget->BindCharacterStat(CharacterStat);
	//}

	//if (!IsPlayerControlled())
	//{
	//	auto DefaultSetting = GetDefault<USPCharacterSetting>();
	//	int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	//	CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

	//	auto SPGameInstance = Cast<USPGameInstance>(GetGameInstance());
	//	if (nullptr != SPGameInstance)
	//	{
	//		AssetStreamingHandle = SPGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &ASPCharacter::OnAssetLoadCompleted));
	//	}
	//}



	bIsPlayer = IsPlayerControlled();
	if (bIsPlayer)
	{
		SPPlayerController = Cast<ASPPlayerController>(GetController());
		ABCHECK(nullptr != SPPlayerController);
	}
	else
	{
		SPAIController = Cast<ASPAIController>(GetController());
		ABCHECK(nullptr != SPAIController);
	}

	auto DefaultSetting = GetDefault<USPCharacterSetting>();

	if (bIsPlayer)
	{
		auto SPPlayerState = Cast<ASPPlayerState>(GetPlayerState());
		ABCHECK(nullptr != SPPlayerState);
		AssetIndex = SPPlayerState->GetCharacterIndex();
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto SPGameInstance = Cast<USPGameInstance>(GetGameInstance());
	ABCHECK(nullptr != SPGameInstance);
	AssetStreamingHandle = SPGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &ASPCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);
}

void ASPCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		//SpringArm->TargetArmLength = 450.0f;
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.0f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::DIABLO:
		//SpringArm->TargetArmLength = 800.0f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0, 0.0f, 0.0f));
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
		break;
	}
}

// Called every frame
void ASPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		break;
	case EControlMode::DIABLO:
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
		break;
	}

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		break;
	case EControlMode::DIABLO:
		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}
}

void ASPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SPAnim = Cast<USPAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != SPAnim);

	SPAnim->OnMontageEnded.AddDynamic(this, &ASPCharacter::OnAttackMontageEnded);

	SPAnim->OnNextAttackCheck.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			SPAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	SPAnim->OnAttackHitCheck.AddUObject(this, &ASPCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnHPIsZero"));
		SPAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});	
}

float ASPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCause)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCause);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	//if (FinalDamage > 0.0f)
	//{
	//	SPAnim->SetDeadAnim();
	//	SetActorEnableCollision(false);
	//}

	CharacterStat->SetDamage(FinalDamage);
	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())
		{
			auto instigator = Cast<ASPPlayerController>(EventInstigator);
			ABCHECK(nullptr != instigator, 0.0f);
			instigator->NPCKill(this);
		}
	}

	return FinalDamage;
}

void ASPCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsPlayerControlled())
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}

// Called to bind functionality to input
void ASPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &ASPCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ASPCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ASPCharacter::Attack);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &ASPCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &ASPCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ASPCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ASPCharacter::Turn);
}

bool ASPCharacter::CanSetWeapon()
{
	//return (nullptr == CurrentWeapon);
	return true;
}

void ASPCharacter::SetWeapon(ASPWeapon* NewWeapon)
{
	//ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);

	ABCHECK(nullptr != NewWeapon);

	if (nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void ASPCharacter::UpDown(float NewAxisValue)
{
	//AddMovementInput(GetActorForwardVector(), NewAxisValue);
	
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void ASPCharacter::LeftRight(float NewAxisValue)
{
	//AddMovementInput(GetActorRightVector(), NewAxisValue);
	
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
		
		break;
	}
}

void ASPCharacter::LookUp(float NewAxisValue)
{	
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	case EControlMode::DIABLO:		
		break;
	}
}

void ASPCharacter::Turn(float NewAxisValue)
{	
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	case EControlMode::DIABLO:
		break;
	}
}

void ASPCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetControlMode(EControlMode::GTA);
		break;
	}
}

void ASPCharacter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		SPAnim->PlayAttackMongtage();
		SPAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void ASPCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();

	OnAttackEnd.Broadcast();
}

void ASPCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void ASPCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void ASPCharacter::AttackCheck()
{
	float FinalAttackRange = GetFinalAttackRange();

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult, GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

#if ENABLE_DRAW_DEBUG
	
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 2.0f;

	DrawDebugCapsule(GetWorld(),
		Center, HalfHeight, AttackRadius,
		CapsuleRot, DrawColor, false, DebugLifeTime);

#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

void ASPCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);
	
	SetCharacterState(ECharacterState::READY);
}

