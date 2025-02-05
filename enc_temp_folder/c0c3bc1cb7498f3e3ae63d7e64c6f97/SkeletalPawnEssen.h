#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SkeletalPawnEssen.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class WORK_7_API ASkeletalPawnEssen : public APawn
{
	GENERATED_BODY()

public:

	ASkeletalPawnEssen();

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


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartBoost(const FInputActionValue& value);
	UFUNCTION()
	void StopBoost(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
};
