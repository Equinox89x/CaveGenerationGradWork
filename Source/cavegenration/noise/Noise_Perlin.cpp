// Fill out your copyright notice in the Description page of Project Settings.


#include "Noise_Perlin.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"

// Sets default values for this component's properties
UNoise_Perlin::UNoise_Perlin()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNoise_Perlin::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UNoise_Perlin::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

