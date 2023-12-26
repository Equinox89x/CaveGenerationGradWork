// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_MetaBall.h"
#include <cavegenration/Generator/NoiseGenerator.h>
#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"


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
		const auto metaBall{ GetWorld()->SpawnActor<AA_Metaball>(MetaBalls[i]) };
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

			for (const AA_Metaball* metaBall : MetaBallObjects)
			{
				const auto pos{ metaBall->GetActorLocation() };
				const float XDiff = cubeCornerPos.X - pos.X;
				const float YDiff = cubeCornerPos.Y - pos.Y;
				const float ZDiff = cubeCornerPos.Z - pos.Z;

				float D = FMath::Sqrt((XDiff * XDiff) + (YDiff * YDiff) + (ZDiff * ZDiff));
				Sum += InfluenceRadius / D;
			}

			if (Sum > NoiseGenerator->ColorToWatch) {
				item.Value.PointValues[j] = NoiseGenerator->white;
			}
			else {
				item.Value.PointValues[j] = NoiseGenerator->black;
			}
			MinValue = FMath::Min(MinValue, Sum);
			MaxValue = FMath::Max(MaxValue, Sum);
		}
	}


	//for (auto& cube : NoiseGenerator->GridCubes)
	//{
	//	for (size_t j = 0; j < cube.Value.PointValues.Num(); j++) {
	//		const int val{ RemapValue(cube.Value.PointValues[j], MinValue, MaxValue, 0, 255) };
	//		cube.Value.PointValues[j] = val > NoiseGenerator->ColorToWatch ? NoiseGenerator->white : NoiseGenerator->black;
	//	}
	//}

}


bool UALG_MetaBall::IsPointInsideSphere(const FVector& Point, const FVector& SphereCenter, float SphereRadius)
{
	const float DistanceSquared = FVector::DistSquared(Point, SphereCenter);
	const float RadiusSquared = FMath::Square(SphereRadius);

	return DistanceSquared <= RadiusSquared;
}

int UALG_MetaBall::RemapValue(int Value, int OldMin, int OldMax, int NewMin, int NewMax)
{
	return FMath::RoundToInt((Value - OldMin) * (float(NewMax - NewMin) / float(OldMax - OldMin)) + NewMin);
}

void UALG_MetaBall::Draw()
{
	Super::Draw();
}