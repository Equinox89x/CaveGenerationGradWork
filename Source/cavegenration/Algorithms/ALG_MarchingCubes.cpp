// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_MarchingCubes.h"
#include <cavegenration/Generator/NoiseGenerator.h>

UALG_MarchingCubes::UALG_MarchingCubes()
{
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void UALG_MarchingCubes::BeginPlay()
{
	Super::BeginPlay();
	NoiseGenerator->DefaultTimer = 1;

}

// Called every frame
void UALG_MarchingCubes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UALG_MarchingCubes::Update()
{
	Super::Update();
    
}

void UALG_MarchingCubes::Draw()
{
	Super::Draw();
}
