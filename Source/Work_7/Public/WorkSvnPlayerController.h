

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WorkSvnPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class WORK_7_API AWorkSvnPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	AWorkSvnPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* UpDownAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* DegreeAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* BoostAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* WheelAction;

	virtual void BeginPlay() override;
};
