// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkSvnPlayerController.h"
#include "EnhancedInputSubsystems.h"

AWorkSvnPlayerController::AWorkSvnPlayerController()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),
	UpDownAction(nullptr),
	DegreeAction(nullptr),
	BoostAction(nullptr),
	JumpAction(nullptr),
	WheelAction(nullptr)
{
}

void AWorkSvnPlayerController::BeginPlay()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}