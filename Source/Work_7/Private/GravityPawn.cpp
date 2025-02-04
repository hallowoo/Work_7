#include "GravityPawn.h"
#include "WorkSvnPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <cmath>


AGravityPawn::AGravityPawn()
{
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

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
	InertiaVelocity = FVector(0.0f, 0.0f, 0.0f);
	CurrentInputDirection = FVector(0.0f, 0.0f, 0.0f);

	JumpSpeed = 600.0f;
	ZSpeed = 0.0f;
	FallingSpeed = 1.0f;

	DragCoefficient = 0.5f;
	SectionalArea = 1.0f;
	Gravity = 980.0f;
	Weight = 50.0f;
	TerminalVelocity = sqrtf((2.0f * Gravity * Weight)/(DragCoefficient * AirDensity * SectionalArea));
}

void AGravityPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AGravityPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsJumping)
	{
		GetGravity(DeltaTime);
		Falling();
	}

}

void AGravityPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&AGravityPawn::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AGravityPawn::StopMove
				);
			}

			if (PlayerController->DegreeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DegreeAction,
					ETriggerEvent::Triggered,
					this,
					&AGravityPawn::Look
				);
			}

			if (PlayerController->BoostAction)
			{
				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Triggered,
					this,
					&AGravityPawn::StartBoost
				);

				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Completed,
					this,
					&AGravityPawn::StopBoost
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AGravityPawn::StartJump
				);
			}

		}
	}
}

void AGravityPawn::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{

		float PitchValue = GetActorRotation().Pitch;
		FRotator ControlRotation = GetControlRotation();
		ControlRotation.Pitch = PitchValue * -1.0f;
		FVector ForwardVector = ControlRotation.Vector();
		AddActorLocalOffset(ForwardVector * MoveInput.X * TotalSpeed);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		FVector RightVector = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
		AddActorLocalOffset(RightVector * MoveInput.Y * TotalSpeed);
	}

	if (!MoveInput.IsNearlyZero())
	{
		float PitchValue = GetActorRotation().Pitch;
		FRotator ControlRotation = GetControlRotation();
		ControlRotation.Pitch = PitchValue * -1.0f;
		FVector ForwardVector = ControlRotation.Vector();
		FVector RightVector = FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
		CurrentInputDirection = (ForwardVector * MoveInput.X + RightVector * MoveInput.Y).GetSafeNormal();
	}
}

void AGravityPawn::StopMove(const FInputActionValue& value)
{
	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero())
	{
		CurrentInputDirection = FVector(0.0f);
	}
}


void AGravityPawn::Look(const FInputActionValue& value)
{
	const FVector2D RotateInput = value.Get<FVector2D>();

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += RotateInput.X;
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - RotateInput.Y, -80.0f, 80.0f);
	SetActorRotation(NewRotation);
}

void AGravityPawn::StartBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed * BoostValue;
}

void AGravityPawn::StopBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed;
}

void AGravityPawn::StartJump(const FInputActionValue& value)
{

	if (!bIsFalling)
	{
		bIsJumping = true;
		ZSpeed = JumpSpeed;
		InertiaVelocity = CurrentInputDirection * TotalSpeed;
	}
}

void AGravityPawn::GetGravity(float DeltaTime)
{
	FallingSpeed = TerminalVelocity * tanh((Gravity / TerminalVelocity) * DeltaTime);
}

void AGravityPawn::Falling()
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