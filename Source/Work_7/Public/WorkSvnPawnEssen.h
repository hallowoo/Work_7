#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WorkSvnPawnEssen.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class WORK_7_API AWorkSvnPawnEssen : public APawn
{
	GENERATED_BODY()

public:

	AWorkSvnPawnEssen();

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
