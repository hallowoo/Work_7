#include "WorkSvnPawn.h"
#include "WorkSvnPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AWorkSvnPawn::AWorkSvnPawn()
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

	NormalSpeed = 40.0f;
	MaxSpeed = NormalSpeed;
	BoostValue = 1.7f;
	CurrentSpeed = 0.0f;
	CurrentUpSpeed = 0.0f;

	CurrentInputDirection = FVector(0.0f, 0.0f, 0.0f);
	DecelerationTime = 0.0f;
	ZDecelTime = 0.0f;
	bIsMoveEnd = true;
	bShouldCountDecelTime = false;
	bShouldCountUpDecelTime = false;
	bIsUpDownEnd = true;
}

void AWorkSvnPawn::BeginPlay()
{
	Super::BeginPlay();	
}

void AWorkSvnPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	if (bShouldCountDecelTime && !bIsMoveEnd)
	{
		DecelerationTime += DeltaTime;
		float Alpha = FMath::Clamp(DecelerationTime / 0.5f, 0.0f, 1.0f);
		float DecelSpeed = FMath::InterpEaseOut(CurrentSpeed, 0.0f, Alpha, 2.0f);
		AddActorLocalOffset(CurrentInputDirection * DecelSpeed, true, &HitResult);
		if (FMath::IsNearlyZero(DecelSpeed))
		{
			bShouldCountDecelTime = false;
			DecelerationTime = 0;
			bIsMoveEnd = true;
		}
	}
	
	if (bShouldCountUpDecelTime && !bIsUpDownEnd)
	{
		ZDecelTime += DeltaTime;
		float Alpha = FMath::Clamp(ZDecelTime / 0.5f, 0.0f, 1.0f);
		float DecelSpeed = FMath::InterpEaseOut(CurrentUpSpeed, 0.0f, Alpha, 2.0f);
		if (HitResult.IsValidBlockingHit() && DecelSpeed < 0)
		{
			DecelSpeed = 0.0f;
		}
		AddActorWorldOffset(FVector(0.0f, 0.0f, DecelSpeed), true, &HitResult);
		if (FMath::IsNearlyZero(DecelSpeed))
		{
			bShouldCountUpDecelTime = false;
			ZDecelTime = 0;
			bIsUpDownEnd = true;
		}
	}

}

void AWorkSvnPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&AWorkSvnPawn::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AWorkSvnPawn::MoveEnd
				);
			}

			if (PlayerController->UpDownAction)
			{
				EnhancedInput->BindAction(
					PlayerController->UpDownAction,
					ETriggerEvent::Triggered,
					this,
					&AWorkSvnPawn::UpDown
				);

				EnhancedInput->BindAction(
					PlayerController->UpDownAction,
					ETriggerEvent::Completed,
					this,
					&AWorkSvnPawn::UpDownEnd
				);
			}

			if (PlayerController->DegreeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DegreeAction,
					ETriggerEvent::Triggered,
					this,
					&AWorkSvnPawn::Degree
				);
			}

			if (PlayerController->BoostAction)
			{
				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Triggered,
					this,
					&AWorkSvnPawn::StartBoost
				);

				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Completed,
					this,
					&AWorkSvnPawn::StopBoost
				);
			}

			if (PlayerController->WheelAction)
				EnhancedInput->BindAction(
					PlayerController->WheelAction,
					ETriggerEvent::Triggered,
					this,
					&AWorkSvnPawn::Zoom
				);
		}
	}
}

void AWorkSvnPawn::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.X < 0)
	{
		return;
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		bIsMoveEnd = false;
		DecelerationTime = 0;
		bShouldCountDecelTime = false;

		FRotator ActorRotation = GetActorRotation();
		FRotator ControlRotation = GetControlRotation();

		ControlRotation.Pitch = -ActorRotation.Pitch;
		ControlRotation.Roll = -ActorRotation.Roll;

		FVector ForwardVector = ControlRotation.Vector();
		FVector RightVector = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y);

		FVector ForwardOffset = ForwardVector * MoveInput.X * MaxSpeed * -(ActorRotation.Pitch / 40.0f);
		FVector RightOffset = RightVector * MoveInput.X * MaxSpeed * (ActorRotation.Roll / 40.0f);

		AddActorLocalOffset(ForwardOffset, true, &HitResult);
		AddActorLocalOffset(RightOffset, true, &HitResult);

		CurrentInputDirection = (ForwardVector * MoveInput.X * -(ActorRotation.Pitch / 40.0f) + RightVector * MoveInput.X * (ActorRotation.Roll / 40.0f)).GetSafeNormal();
		CurrentSpeed = (ForwardOffset + RightOffset).Size();
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddActorWorldRotation(FRotator(0.0f, MoveInput.Y * (NormalSpeed/10), 0.0f));
	}
}

void AWorkSvnPawn::MoveEnd(const FInputActionValue& value)
{
	const FVector2D MoveInput = value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero())
	{
		bShouldCountDecelTime = true;

	}
}

void AWorkSvnPawn::UpDown(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}

	const float UpDownInput = value.Get<float>()*MaxSpeed;

	if (!FMath::IsNearlyZero(UpDownInput))
	{		
		bIsUpDownEnd = false;
		bShouldCountUpDecelTime = false;
		ZDecelTime = 0.0f;
		if (HitResult.IsValidBlockingHit() && UpDownInput < 0)
		{
			return;
		}
		FRotator ActorRotation = GetActorRotation();
		FVector UpVector = FRotationMatrix(ActorRotation).GetUnitAxis(EAxis::Z);
		float ZAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(UpVector, FVector::UpVector)));
		CurrentUpSpeed = UpDownInput * ((50.0f - ZAngle) / 50.0f);
		AddActorWorldOffset(FVector(0.0f, 0.0f, CurrentUpSpeed), true, &HitResult);
	}

}

void AWorkSvnPawn::UpDownEnd(const FInputActionValue& value)
{
	const float UpDownInput = value.Get<float>();
	if (FMath::IsNearlyZero(UpDownInput))
	{
		bShouldCountUpDecelTime = true;
	}
}

void AWorkSvnPawn::Degree(const FInputActionValue& value)
{	
	const FVector2D RotateInput = value.Get<FVector2D>();

	FRotator NewRotation = GetActorRotation();
	NewRotation.Roll = FMath::Clamp(NewRotation.Roll - RotateInput.X, -40.0f, 40.0f);
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - RotateInput.Y, -40.0f, 40.0f);
	SetActorRotation(NewRotation);
}

void AWorkSvnPawn::StartBoost(const FInputActionValue& value)
{
	MaxSpeed = NormalSpeed * BoostValue;
}

void AWorkSvnPawn::StopBoost(const FInputActionValue& value)
{
	MaxSpeed = NormalSpeed;
}

void AWorkSvnPawn::Zoom(const FInputActionValue& value)
{
	float ScrollValue = value.Get<float>();

	if (!FMath::IsNearlyZero(ScrollValue))
	{
		SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + (ScrollValue*20.0f), 100.0f, 700.0f);
	}
}