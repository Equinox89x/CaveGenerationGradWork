// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_MetaBall.h"
#include <cavegenration/Generator/NoiseGenerator.h>


// Sets default values for this component's properties
UALG_MetaBall::UALG_MetaBall()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
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

	for (size_t i = 0; i < MetaBalls.Num(); i++)
	{
		auto metaBall{ GetWorld()->SpawnActor<AA_Metaball>(MetaBalls[i]) };
		FVector loc{ MinBoundary }; 
		loc.X += 300;
		loc.Y += 300;
		metaBall->InitValues(MinBoundary, MaxBoundary, loc);
		MetaBallObjects.Add(metaBall);
	}
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

	InfluenceRadius = sumModifier * sumModifier2;

	//for (size_t i = 0; i < NoiseGenerator->GridSize.X; i++)
	//{
	//	for (size_t j = 0; j < NoiseGenerator->GridSize.Y; j++)
	//	{
	//		for (size_t k = 0; k < NoiseGenerator->GridSize.Z; k++)
	//		{
	//			int32 Index = i + j * NoiseGenerator->GridSize.X + k * NoiseGenerator->GridSize.X * NoiseGenerator->GridSize.Y;
	//			auto visPos{ NoiseGenerator->McData.Locations[Index] };
	//			float Sum = 0;

	//			for (int32 l = 0; l < MetaBallObjects.Num(); l++)
	//			{
	//				auto pos{ MetaBallObjects[l]->GetActorLocation() };
	//				float XDiff = visPos.X - pos.X;
	//				float YDiff = visPos.Y - pos.Y;
	//				float ZDiff = visPos.Z - pos.Z;

	//				float D = FMath::Sqrt((XDiff * XDiff) + (YDiff * YDiff) + (ZDiff * ZDiff));
	//				Sum += InfluenceRadius / D;
	//				//Sum += sumModifier * sumModifier2 / D;
	//			}

	//			NoiseGenerator->McData.Values[Index] = Sum;
	//		}
	//	}
	//}

	float minValue = 0;
	float maxValue = 0;

	for (FMCCube& cube : NoiseGenerator->GridCubes)
	{
		for (size_t j = 0; j < cube.PointPositions.Num(); j++)
		{
			float Sum = 0;
			auto cubeCornerPos{ cube.PointPositions[j] };

			for (const AA_Metaball* metaBall : MetaBallObjects)
			{
				auto pos{ metaBall->GetActorLocation() };
				float XDiff = cubeCornerPos.X - pos.X;
				float YDiff = cubeCornerPos.Y - pos.Y;
				float ZDiff = cubeCornerPos.Z - pos.Z;

				float D = FMath::Sqrt((XDiff * XDiff) + (YDiff * YDiff) + (ZDiff * ZDiff));
				Sum += InfluenceRadius / D;
			}

			cube.PointValues[j] = FMath::Clamp(Sum, 0, 255);
			minValue = FMath::Min(minValue, cube.PointValues[j]);
			maxValue = FMath::Max(maxValue, cube.PointValues[j]);
		}
	}

	//for (FMCCube& cube : NoiseGenerator->GridCubes)
	//{
	//	for (size_t j = 0; j < cube.PointValues.Num(); j++) {
	//		//float MappedSum = (cube.PointValues[j] - minValue) / (maxValue - minValue) * 255.0f;
	//		cube.PointValues[j] = RemapValue(cube.PointValues[j], minValue, maxValue, 0, 255);
	//	}
	//}

	//multithread
}

int UALG_MetaBall::RemapValue(int Value, int OldMin, int OldMax, int NewMin, int NewMax)
{
	return FMath::RoundToInt((Value - OldMin) * (float(NewMax - NewMin) / float(OldMax - OldMin)) + NewMin);
}

void UALG_MetaBall::Draw()
{
	Super::Draw();
}