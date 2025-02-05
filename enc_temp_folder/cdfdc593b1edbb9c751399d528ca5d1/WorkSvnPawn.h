#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WorkSvnPawn.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class WORK_7_API AWorkSvnPawn : public APawn
{
	GENERATED_BODY()

public:

	AWorkSvnPawn();

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
	float MaxSpeed;

	FHitResult HitResult;
	FVector CurrentInputDirection;
	float DecelerationTime;
	bool bIsMoveEnd;
	bool bShouldCountDecelTime;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void MoveEnd(const FInputActionValue& value);
	UFUNCTION()
	void UpDown(const FInputActionValue& value);
	UFUNCTION()
	void Degree(const FInputActionValue& value);
	UFUNCTION()
	void StartBoost(const FInputActionValue& value);
	UFUNCTION()
	void StopBoost(const FInputActionValue& value);
	UFUNCTION()
	void Zoom(const FInputActionValue& value);
};
