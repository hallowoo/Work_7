#include "WorkSvnPawnEssen.h"
#include "WorkSvnPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AWorkSvnPawnEssen::AWorkSvnPawnEssen()
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

	NormalSpeed = 20.0f;
	BoostValue = 1.7f;
	TotalSpeed = NormalSpeed;

}

void AWorkSvnPawnEssen::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
					&AWorkSvnPawnEssen::Move
				);
			}

			if (PlayerController->BoostAction)
			{
				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Triggered,
					this,
					&AWorkSvnPawnEssen::StartBoost
				);

				EnhancedInput->BindAction(
					PlayerController->BoostAction,
					ETriggerEvent::Completed,
					this,
					&AWorkSvnPawnEssen::StopBoost
				);
			}

			if (PlayerController->DegreeAction)
			{
				EnhancedInput->BindAction(
					PlayerController->DegreeAction,
					ETriggerEvent::Triggered,
					this,
					&AWorkSvnPawnEssen::Look
				);
			}
		}
	}
}

void AWorkSvnPawnEssen::Move(const FInputActionValue& value)
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
}

void AWorkSvnPawnEssen::StartBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed * BoostValue;
}

void AWorkSvnPawnEssen::StopBoost(const FInputActionValue& value)
{
	TotalSpeed = NormalSpeed;
}

void AWorkSvnPawnEssen::Look(const FInputActionValue& value)
{
	const FVector2D RotateInput = value.Get<FVector2D>();

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += RotateInput.X;
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch - RotateInput.Y, -80.0f, 80.0f);
	SetActorRotation(NewRotation);
}
