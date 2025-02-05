// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalPawnEssen.h"

// Sets default values
ASkeletalPawnEssen::ASkeletalPawnEssen()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkeletalPawnEssen::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkeletalPawnEssen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASkeletalPawnEssen::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

