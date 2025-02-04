#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GravityPawn.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class WORK_7_API AGravityPawn : public APawn
{
	GENERATED_BODY()

public:

	AGravityPawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float BoostValue;
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
