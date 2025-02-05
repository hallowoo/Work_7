#include "SkeletalPawnGravity.h"
#include "WorkSvnPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <cmath>

ASkeletalPawnGravity::ASkeletalPawnGravity()
{
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BoxCollision"));
	SetRootComponent(CapsuleCollision);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = true;

	bIsJumping = false;
	bIsFalling = false;

	NormalSpeed = 20.0f;
	BoostValue = 1.7f;
	TotalSpeed = NormalSpeed;
	MoveVelocity = 0.0f;
	InertiaVelocity = FVector(0.0f, 0.0f, 0.0f);
	CurrentInputDirection = FVector(0.0f, 0.0f, 0.0f);

	JumpSpeed = 600.0f;
	ZSpeed = 0.0f;
	FallingSpeed = 1.0f;

	DragCoefficient = 0.5f;
	SectionalArea = 1.0f;
	Gravity = 980.0f;
	Weight = 50.0f;
	TerminalVelocity = sqrtf((2.0f * Gravity * Weight) / (DragCoefficient * AirDensity * SectionalArea));
}

void ASkeletalPawnGravity::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkeletalPawnGravity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsJumping)
	{
		GetGravity(DeltaTime);
		Falling();
	}

}

void ASkeletalPawnGravity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AWorkSvnPlayerController* PlayerController = Cast<AWorkSvnPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASkeletalPawnGravity::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&ASkeletalPawnGravity::StopMove
				);
			}

			if (PlayerController->DegreeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DegreeAction,
					ETriggerEvent::Triggered,
					this,
					&ASkeletalPawnGravity::Look
				);
			}

			if (PlayerController->BoostAction)
			{
				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Triggered,
					this,
					&ASkeletalPawnGravity::StartBoost
				);

				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Completed,
					this,
					&ASkeletalPawnGravity::StopBoost
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASkeletalPawnGravity::StartJump
				);
			}

		}
	}
}

void ASkeletalPawnGravity::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveInput = value.Get<FVector2D>();
	float PitchValue = GetActorRotation().Pitch;
	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = PitchValue * -1.0f;
	FVector ForwardVector = ControlRotation.Vector();
	FVector RightVector = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
	FVector ForwardOffset = ForwardVector * MoveInput.X * TotalSpeed;
	FVector RightOffset = RightVector * MoveInput.Y * TotalSpeed;


	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddActorLocalOffset(ForwardVector * MoveInput.X * TotalSpeed);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddActorLocalOffset(RightVector * MoveInput.Y * TotalSpeed);
	}
	SkeletalMesh->SetRelativeRotation(FRotator(0.0f, ControlRotation.Yaw - 90.0f, 0.0f));
	MoveVelocity = (ForwardOffset + RightOffset).Size();

	if (MoveInput.IsNearlyZero())
	{
		MoveVelocity = 0.0f;
	}
}

void ASkeletalPawnGravity::StopMove(const FInputActionValue& value)
{
	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero())
	{
		CurrentInputDirection = FVector(0.0f);
	}
}


void ASkeletalPawnGravity::Look(const FInputActionValue& value)
{
	const FVector2D RotateInput = value.Get<FVector2D>();

	AddControllerYawInput(RotateInput.X);
	AddControllerPitchInput(RotateInput.Y);
}

void ASkeletalPawnGravity::StartBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed * BoostValue;
}

void ASkeletalPawnGravity::StopBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed;
}

void ASkeletalPawnGravity::StartJump(const FInputActionValue& value)
{

	if (!bIsFalling)
	{
		bIsJumping = true;
		ZSpeed = JumpSpeed;
		InertiaVelocity = CurrentInputDirection * TotalSpeed;
	}
}

void ASkeletalPawnGravity::GetGravity(float DeltaTime)
{
	FallingSpeed = TerminalVelocity * tanh((Gravity / TerminalVelocity) * DeltaTime);
}

void ASkeletalPawnGravity::Falling()
{
	bIsFalling = true;
	AddActorLocalOffset(InertiaVelocity);
	TotalSpeed = NormalSpeed * 0.5f;

	ZSpeed -= FallingSpeed;
	float PitchValue = GetActorRotation().Pitch;

	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = PitchValue * -1.0f + 90.0f;
	FVector UpVector = ControlRotation.Vector();

	AddActorLocalOffset(UpVector * ZSpeed, true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		ZSpeed = 0;
		TotalSpeed = NormalSpeed;
		bIsFalling = false;
		bIsJumping = false;
	}
}