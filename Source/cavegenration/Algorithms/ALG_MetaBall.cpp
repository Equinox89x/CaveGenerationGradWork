// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_MetaBall.h"
#include <cavegenration/Generator/NoiseGenerator.h>
#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"


UALG_MetaBall::UALG_MetaBall()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UALG_MetaBall::BeginPlay()
{
	Super::BeginPlay();
}

void UALG_MetaBall::InitValues(FVector minBoundary, FVector maxBoundary)
{
	Super::InitValues(minBoundary, maxBoundary);

	if (!MetaBallObjects.IsEmpty()) return;
	MinBoundary = minBoundary;
	MaxBoundary = maxBoundary;

	GenerateMetaBalls();
}

void UALG_MetaBall::Cleanup()
{
	Super::Cleanup();

	for (AA_Metaball* metaBall : MetaBallObjects)
	{
		metaBall->Destroy();
	}
	MetaBallObjects.Empty();
}

// Called every frame
void UALG_MetaBall::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UALG_MetaBall::Update()
{
	Super::Update();

	MinValue = 0;
	MaxValue = 0;
	SphereSize = InfluenceRadius/2;
	
	const int NumCubes = NoiseGenerator->GridCubes.Num();

	TArray<FGraphEventRef> TaskRefs;
	for(auto& item: NoiseGenerator->GridCubes){
	//for (int i = 0; i < NoiseGenerator->GridCubes.Num() - 1; ++i)
	//{
	//	FMCCube& item{ NoiseGenerator->GridCubes[i] };
		for (size_t j = 0; j < item.Value.PointPositions.Num(); j++)
		{
			float Sum = 0;
			const auto cubeCornerPos{ item.Value.PointPositions[j] };

			int layerNr;
			float largestNr{ 0 };
			for (const AA_Metaball* metaBall : MetaBallObjects)
			{
				const auto pos{ metaBall->GetActorLocation() };
				const float XDiff = (cubeCornerPos.X - pos.X) / MinRadius;
				const float YDiff = (cubeCornerPos.Y - pos.Y) / MinRadius;
				const float ZDiff = (cubeCornerPos.Z - pos.Z) / MinRadius;

				float D = FMath::Sqrt((XDiff * XDiff) + (YDiff * YDiff) + (ZDiff * ZDiff));
				Sum += InfluenceRadius / D;

				if (D > largestNr) {
					largestNr = D;
					layerNr = metaBall->LayerNr;
				}
			}

			if (CanAssignValues) {
				if (Sum > NoiseGenerator->BiomeGenerator->Treshold) {
					item.Value.PointValues[j] = NoiseGenerator->white;
				}
				else {
					//item.Value.PointValues[j] = NoiseGenerator->black;
				}
			}
			MinValue = FMath::Min(MinValue, Sum);
			MaxValue = FMath::Max(MaxValue, Sum);

			if (layerNr <= NoiseGenerator->BiomeGenerator->BiomeMaterials.Num()) {
				auto val{ NoiseGenerator->BiomeGenerator->FindClosestFloat(layerNr, true) };
				item.Value.BiomeValues.Init(val, 8);
			}
			else {
				item.Value.BiomeValues.Init(0, 8);
			}
		}
	}
}


bool UALG_MetaBall::IsPointInsideSphere(const FVector& Point, const FVector& SphereCenter, float SphereRadius)
{
	const float DistanceSquared = FVector::DistSquared(Point, SphereCenter);
	const float RadiusSquared = FMath::Square(SphereRadius);

	return DistanceSquared <= RadiusSquared;
}

void UALG_MetaBall::GenerateMetaBalls()
{
	FVector dimensions{ MaxBoundary - MinBoundary };
	float heightToSpawn{ static_cast<float>(dimensions.Z) / NrOfLayers };
	float calculatedHeight{ static_cast<float>(MinBoundary.Z) };

	for (size_t i = 0; i < NrOfLayers; i++)
	{
		for (size_t j = 0; j < NrOfBallsPerLayer + NrOfHeightBallsPerLayer; j++)
		{
			FVector spawnLocation{
				MinBoundary.X + (static_cast<float>(j % NrOfBallsPerLayer) / (NrOfBallsPerLayer - 1)) * dimensions.X,
				MinBoundary.Y + (static_cast<float>(j / NrOfBallsPerLayer) / (NrOfHeightBallsPerLayer - 1)) * dimensions.Y,
				MinBoundary.Z + calculatedHeight
			};

			const auto metaBall{ GetWorld()->SpawnActor<AA_Metaball>(MetaBallToSpawn) };
			metaBall->InitValues(MinBoundary, MaxBoundary, spawnLocation);

			if (j >= NrOfBallsPerLayer) {
				metaBall->IsHeightBall = true;
			}

			metaBall->LayerNr = i;
			MetaBallObjects.Add(metaBall);
		}
		calculatedHeight += heightToSpawn;

	}
}

int UALG_MetaBall::RemapValue(int Value, int OldMin, int OldMax, int NewMin, int NewMax)
{
	return FMath::RoundToInt((Value - OldMin) * (float(NewMax - NewMin) / float(OldMax - OldMin)) + NewMin);
}

void UALG_MetaBall::Draw()
{
	Super::Draw();
}