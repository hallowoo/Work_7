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

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;

	PrimaryActorTick.bCanEverTick = true;

	NormalSpeed = 20.0f;
	MaxSpeed = NormalSpeed;
	BoostValue = 1.7f;

	CurrentInputDirection = FVector(0.0f, 0.0f, 0.0f);
	DecelerationTime = 0;
	bIsMoveEnd = true;
	bShouldCountDecelTime = false;
}

void AWorkSvnPawn::BeginPlay()
{
	Super::BeginPlay();	
}

void AWorkSvnPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	if (bShouldCountDecelTime)
	{
		DecelerationTime += DeltaTime;
		float Alpha = FMath::Clamp(DecelerationTime / 1.0f, 0.0f, 1.0f);
		float DecelSpeed = FMath::InterpEaseOut(NormalSpeed, 0.0f, Alpha, 2.0f);
		AddActorLocalOffset(CurrentInputDirection * DecelSpeed, true, &HitResult);
		if (FMath::IsNearlyZero(DecelSpeed))
		{
			bShouldCountDecelTime = false;
			DecelerationTime = 0;
			bIsMoveEnd = true;
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
		bShouldCountDecelTime = false;
		FRotator ActorRotation = GetActorRotation();
		FRotator ControlRotation = GetControlRotation();
		ControlRotation.Pitch = -ActorRotation.Pitch;
		FVector ForwardVector = ControlRotation.Vector();

		ControlRotation.Roll = -ActorRotation.Roll;
		FVector RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);

		AddActorLocalOffset(ForwardVector * MoveInput.X * MaxSpeed * -(ActorRotation.Pitch/40.0f), true, &HitResult);
		//FRotator ActorRotation = GetActorRotation();
		//FRotator ControlRotation = GetControlRotation();

		AddActorLocalOffset(RightVector * MoveInput.X * MaxSpeed* (ActorRotation.Roll/40.0f), true, &HitResult);

		CurrentInputDirection = (ForwardVector * MoveInput.X + RightVector * MoveInput.X).GetSafeNormal();
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddActorWorldRotation(FRotator(0.0f, MoveInput.Y * (NormalSpeed/5), 0.0f));
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
		if (HitResult.IsValidBlockingHit() && UpDownInput < 0)
		{
			return;
		}
		FRotator ControlRotation = GetController()->GetControlRotation();
		ControlRotation.Pitch = ControlRotation.Pitch * -1.0f;
		FVector UpVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Z);

		AddActorWorldOffset(FVector(0.0f, 0.0f, UpDownInput), true, &HitResult);
	}

}

void AWorkSvnPawn::Degree(const FInputActionValue& value)
{	
	const FVector2D RotateInput = value.Get<FVector2D>();

	FRotator NewRotation = GetActorRotation();
	NewRotation.Roll -= RotateInput.X;
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - RotateInput.Y, -80.0f, 80.0f); // 상하 회전 제한
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