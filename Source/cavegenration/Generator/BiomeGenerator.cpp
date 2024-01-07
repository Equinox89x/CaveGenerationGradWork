// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerator.h"
#include "NoiseGenerator.h"

ABiomeGenerator::ABiomeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	InstancedFloorMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedFloorMesh"));
	InstancedCeilingMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedCeilingMesh"));
}

void ABiomeGenerator::BeginPlay()
{
	Super::BeginPlay();
	FillColorArr();
}

void ABiomeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABiomeGenerator::InitNoise() {
	FastNoise.SetNoiseType(SelectedNoise);
	FastNoise.SetSeed(Seed);
	FastNoise.SetFrequency(NoiseScale);
	FastNoise.SetFractalType(FastNoiseLite::FractalType_None);
}

int ABiomeGenerator::GetBiomeNoise(FVector position) {
	auto biomeValue{ FastNoise.GetNoise(position.X, position.Y, position.Z) };
	return FMath::RoundToInt((((biomeValue + 1) / 2) * 255) * NoiseMultiplier);
}

void ABiomeGenerator::InitBiomeLayers(FVector minBoundary, FVector maxBoundary)
{
	FVector dimensions{ maxBoundary - minBoundary };
	float heightToSpawn{ static_cast<float>(dimensions.Z) / NrOfBiomeLayers };
	float calculatedHeight{ static_cast<float>(minBoundary.Z) };

	for (size_t i = 0; i < NrOfBiomeLayers; i++)
	{
		const auto zLoc{ minBoundary.Z + calculatedHeight };
		const auto zLocMin{ zLoc };
		const auto zLocMax{ zLoc + heightToSpawn };
		calculatedHeight += heightToSpawn;
		LayerBiomes.Add({ i, FVector2D{ zLocMin, zLocMax } });
	}
}

void ABiomeGenerator::CreateFloorAndCeiling(FVector Vertex0, FVector Vertex1, FVector Vertex2, FVector color)
{
	FVector centroid = (Vertex0 + Vertex1 + Vertex2) / 3.0f;
	if (IsTriangleLookingUp(Vertex0, Vertex1, Vertex2)) {
		FloorLocations.Add(centroid, color);
	}
	if (IsTriangleLookingDown(Vertex0, Vertex1, Vertex2)) {
		CeilingLocations.Add(centroid, color);
	}
}

void ABiomeGenerator::GenerateBiome()
{
	InstancedFloorMesh->ClearInstances();
	InstancedCeilingMesh->ClearInstances();

	for (const auto& item : FloorLocations) {
		FTransform trans{};
		trans.SetLocation(item.Key);
		trans.SetScale3D(FVector(0.3));
		InstancedFloorMesh->AddInstance(trans, true);
		InstancedFloorMesh->SetCustomDataValue(InstancedFloorMesh->GetInstanceCount() - 1, 0, item.Value.X);
		InstancedFloorMesh->SetCustomDataValue(InstancedFloorMesh->GetInstanceCount() - 1, 1, item.Value.Y);
		InstancedFloorMesh->SetCustomDataValue(InstancedFloorMesh->GetInstanceCount() - 1, 2, item.Value.Z);
	}
	for (const auto& item : CeilingLocations) {
		FVector loc{ item.Key };
		loc.Z -= InstancedCeilingMesh->GetStaticMesh()->GetBounds().BoxExtent.Z/2;

		FTransform trans{};
		trans.SetLocation(loc);
		trans.SetScale3D(FVector(0.3));
		InstancedCeilingMesh->AddInstance(trans, true);
		InstancedCeilingMesh->SetCustomDataValue(InstancedCeilingMesh->GetInstanceCount() - 1, 0, item.Value.X);
		InstancedCeilingMesh->SetCustomDataValue(InstancedCeilingMesh->GetInstanceCount() - 1, 1, item.Value.Y);
		InstancedCeilingMesh->SetCustomDataValue(InstancedCeilingMesh->GetInstanceCount() - 1, 2, item.Value.Z);
	}
}

void ABiomeGenerator::ClearBiome()
{
	InstancedFloorMesh->ClearInstances();
	InstancedCeilingMesh->ClearInstances();
}

void ABiomeGenerator::CycleBiomeNoise()
{
	const int index = (static_cast<int>(SelectedNoise) + 1) % static_cast<int>(FastNoiseLite::NoiseType_Value);
	SelectedNoise = static_cast<FastNoiseLite::NoiseType>(index);

	if (SelectedNoise == FastNoiseLite::NoiseType_Value) {
		SelectedNoise = FastNoiseLite::NoiseType_OpenSimplex2;
	}
}

void ABiomeGenerator::Reset()
{
	FloorLocations.Empty();
	CeilingLocations.Empty();
}

void ABiomeGenerator::FillColorArr()
{
	NrOfColors = BiomeMaterials.Num();
	NoiseBiomes.Empty();
	for (size_t i = 0; i < NrOfColors; i++)
	{
		NoiseBiomes.Add(255 / (i+1), BiomeMaterials[i]);
	}
}

bool ABiomeGenerator::IsTriangleLookingUp(const FVector& Vertex1, const FVector& Vertex2, const FVector& Vertex3)
{
	FVector edge1{ Vertex2 - Vertex1 };
	FVector edge2{ Vertex3 - Vertex1 };

	FVector normal{ FVector::CrossProduct(edge1, edge2).GetSafeNormal() };
	float dotProduct{ static_cast<float>(FVector::DotProduct(normal, FVector::UpVector)) };
	return dotProduct > FloorMaxAngle;
}

bool ABiomeGenerator::IsTriangleLookingDown(const FVector& Vertex1, const FVector& Vertex2, const FVector& Vertex3)
{
	FVector edge1{ Vertex2 - Vertex1 };
	FVector edge2{ Vertex3 - Vertex1 };

	FVector normal{ FVector::CrossProduct(edge1, edge2).GetSafeNormal() };
	float dotProduct{ static_cast<float>(FVector::DotProduct(normal, FVector(0, 0, -1))) };
	return dotProduct > CeilingMaxAngle;
}

float ABiomeGenerator::FindClosestFloat(float TargetNumber, bool isDirectAccess)
{
	TArray<float> floatArray;
	NoiseBiomes.GetKeys(floatArray);

	if (floatArray.Num() == 0)
	{
		return 0.0f;
	}

	if (isDirectAccess) {
		return floatArray[int(TargetNumber)];
	}

	float closestFloat{ floatArray[0] };
	float minDifference = FMath::Abs(TargetNumber - closestFloat);

	for (float currentFloat : floatArray)
	{
		float currentDifference = FMath::Abs(TargetNumber - currentFloat);

		if (currentDifference < minDifference)
		{
			closestFloat = currentFloat;
			minDifference = currentDifference;
		}
	}

	return closestFloat;
}

FBiome& ABiomeGenerator::GetSelectedBiome(const FMCCube& cube, bool isLayeredMethod)
{
	TArray<float> noiseBiomeKeys;
	NoiseBiomes.GetKeys(noiseBiomeKeys);
	if (isLayeredMethod) {
		for (const auto& pair : LayerBiomes) {
			const FVector2D minMax{ pair.Value };
			if (cube.CubeLocation.Z >= minMax.X && cube.CubeLocation.Z <= minMax.Y) {

				return NoiseBiomes[noiseBiomeKeys[pair.Key]];
			}
		}
	}
	else {
		TMap<float, int32> valueCountMap;

		for (float Value : cube.BiomeValues)
		{
			if (valueCountMap.Contains(Value))
			{
				valueCountMap[Value]++;
			}
			else
			{
				valueCountMap.Add(Value, 1);
			}
		}

		float MostFrequentValue;
		int MaxCount{ 0 };
		for (const auto& Pair : valueCountMap)
		{
			if (Pair.Value > MaxCount)
			{
				MaxCount = Pair.Value;
				MostFrequentValue = Pair.Key;
			}
		}

		return NoiseBiomes[MostFrequentValue];
	}
	return NoiseBiomes[noiseBiomeKeys[0]];
}
