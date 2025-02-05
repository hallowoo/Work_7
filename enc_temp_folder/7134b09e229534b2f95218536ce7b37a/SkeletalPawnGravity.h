#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SkeletalPawnGravity.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class WORK_7_API ASkeletalPawnGravity : public APawn
{
	GENERATED_BODY()

public:
	ASkeletalPawnGravity();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float BoostValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Speed")
	float MoveVelocity;
	float TotalSpeed;

	bool bIsJumping;
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float JumpSpeed;
	float ZSpeed;

	float FallingSpeed;

	const float AirDensity = 1.225f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity", meta = (ClampMin = "0.5", ClampMax = "1.2", UIMin = "0.5", UIMax = "1.2"))
	float DragCoefficient;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float SectionalArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float Gravity;
	float TerminalVelocity;
	FVector InertiaVelocity;
	FVector CurrentInputDirection;
	FHitResult HitResult;



	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StopMove(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartBoost(const FInputActionValue& value);
	UFUNCTION()
	void StopBoost(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);

	void GetGravity(float DeltaTime);
	void Falling();

};
