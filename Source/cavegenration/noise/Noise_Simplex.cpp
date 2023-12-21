// Fill out your copyright notice in the Description page of Project Settings.


#include "Noise_Simplex.h"

// Sets default values for this component's properties
UNoise_Simplex::UNoise_Simplex()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNoise_Simplex::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNoise_Simplex::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

