// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerator.h"

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
	auto biomeValue{ FastNoise.GetNoise(position.X, position.Y) };
	return FMath::RoundToInt((((biomeValue + 1) / 2) * 255) * NoiseMultiplier);
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
		loc.Z -= InstancedCeilingMesh->GetStaticMesh()->GetBounds().BoxExtent.Z;

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
	UsableColors.Empty();
	for (size_t i = 0; i < NrOfColors; i++)
	{
		UsableColors.Add(255 / (i+1), BiomeMaterials[i]);
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
	UsableColors.GetKeys(floatArray);

	if (floatArray.Num() == 0)
	{
		return 0.0f;
	}

	if (isDirectAccess) {
		return floatArray[int(TargetNumber)];
	}

	float ClosestFloat{ floatArray[0] };
	float MinDifference = FMath::Abs(TargetNumber - ClosestFloat);

	for (float CurrentFloat : floatArray)
	{
		float CurrentDifference = FMath::Abs(TargetNumber - CurrentFloat);

		if (CurrentDifference < MinDifference)
		{
			ClosestFloat = CurrentFloat;
			MinDifference = CurrentDifference;
		}
	}

	return ClosestFloat;
}

FBiome ABiomeGenerator::GetSelectedBiome(const TArray<float>& biomeValues)
{
	TMap<float, int32> ValueCountMap;

	for (float Value : biomeValues)
	{
		if (ValueCountMap.Contains(Value))
		{
			ValueCountMap[Value]++;
		}
		else
		{
			ValueCountMap.Add(Value, 1);
		}
	}

	float MostFrequentValue;
	int MaxCount{ 0 };
	for (const auto& Pair : ValueCountMap)
	{
		if (Pair.Value > MaxCount)
		{
			MaxCount = Pair.Value;
			MostFrequentValue = Pair.Key;
		}
	}

	return UsableColors[MostFrequentValue];
}
