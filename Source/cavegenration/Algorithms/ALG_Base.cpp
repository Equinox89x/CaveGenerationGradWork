// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_Base.h"
#include <cavegenration/Generator/NoiseGenerator.h>

UALG_Base::UALG_Base()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UALG_Base::BeginPlay()
{
	Super::BeginPlay();

	NoiseGenerator = Cast<ANoiseGenerator>(GetOwner());
}

void UALG_Base::Update() {
	timer -= deltatime;
}

void UALG_Base::Draw()
{

}

void UALG_Base::HandlePreMeshGen()
{
	NoiseGenerator->GridCubesForMesh.Empty();
	TArray<FVector> keys2;
	NoiseGenerator->GridCubes.GetKeys(keys2);
	for (const auto& key : keys2)
	{
		const auto& cube = NoiseGenerator->GridCubes[key];
		bool isNotFullOrEmpty{ cube.PointValues.Contains(0) && cube.PointValues.Contains(255) };
		if (isNotFullOrEmpty) {
			NoiseGenerator->GridCubesForMesh.Add(cube.CubeLocation, cube);
		}
	}
}

void UALG_Base::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	deltatime = DeltaTime;
}
