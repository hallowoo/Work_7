#include "SkeletalPawnEssen.h"
#include "WorkSvnPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASkeletalPawnEssen::ASkeletalPawnEssen()
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

	NormalSpeed = 20.0f;
	BoostValue = 1.7f;
	TotalSpeed = NormalSpeed;
	MoveVelocity = 0.0f;
}

void ASkeletalPawnEssen::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&ASkeletalPawnEssen::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&ASkeletalPawnEssen::Move
				);
			}

			if (PlayerController->BoostAction)
			{
				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Triggered,
					this,
					&ASkeletalPawnEssen::StartBoost
				);

				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Completed,
					this,
					&ASkeletalPawnEssen::StopBoost
				);
			}

			if (PlayerController->DegreeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DegreeAction,
					ETriggerEvent::Triggered,
					this,
					&ASkeletalPawnEssen::Look
				);
			}
		}
	}
}

void ASkeletalPawnEssen::Move(const FInputActionValue& value)
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

void ASkeletalPawnEssen::StartBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed * BoostValue;
}

void ASkeletalPawnEssen::StopBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed;
}

void ASkeletalPawnEssen::Look(const FInputActionValue& value)
{
	const FVector2D RotateInput = value.Get<FVector2D>();

	AddControllerYawInput(RotateInput.X);
	AddControllerPitchInput(RotateInput.Y);
}
