// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <cavegenration/Algorithms/ALG_CellularAutomata.h>
#include <cavegenration/GameEnums.h>
#include <cavegenration/Algorithms/ALG_MetaBall.h>
#include <cavegenration/Algorithms/ALG_MarchingCubes.h>
#include "BiomeGenerator.h"
#include "NoiseGenerator.generated.h"

#pragma region structs
USTRUCT(BlueprintType)
struct FMCCubeIndex {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector vertLocation;	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector cubeLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int vertIndex;
};

USTRUCT(BlueprintType)
struct FMCTriangle {
	GENERATED_BODY()
	FMCTriangle() {};
	FMCTriangle(int p1, int p2, FVector loc) {
		pointOne = p1;
		pointTwo = p2;
		Position = loc;
		OriginalPosition = loc;
	};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int pointOne;	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int pointTwo;		
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Position;		
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector OriginalPosition;	
};

USTRUCT(BlueprintType)
struct FMCCube {
	GENERATED_BODY()

	FMCCube() {};

	FMCCube(FVector location, float cubeSize, int index) {
		CubeLocation = location;
		CubeSize = cubeSize;

		FVector Extents{ cubeSize, cubeSize, cubeSize };
		PointPositions.Add(CubeLocation + FVector(-Extents.X, -Extents.Y, -Extents.Z));
		PointPositions.Add(CubeLocation + FVector(Extents.X, -Extents.Y, -Extents.Z));
		PointPositions.Add(CubeLocation + FVector(Extents.X, Extents.Y, -Extents.Z));
		PointPositions.Add(CubeLocation + FVector(-Extents.X, Extents.Y, -Extents.Z));
		PointPositions.Add(CubeLocation + FVector(-Extents.X, -Extents.Y, Extents.Z));
		PointPositions.Add(CubeLocation + FVector(Extents.X, -Extents.Y, Extents.Z));
		PointPositions.Add(CubeLocation + FVector(Extents.X, Extents.Y, Extents.Z));
		PointPositions.Add(CubeLocation + FVector(-Extents.X, Extents.Y, Extents.Z));

		// Calculate midpoints and add them to the array
		TrianglePointPositions.Add(FMCTriangle{ 0, 1, (PointPositions[0] + PointPositions[1]) * 0.5f }); // Edge 0-1
		TrianglePointPositions.Add(FMCTriangle{ 1, 2, (PointPositions[1] + PointPositions[2]) * 0.5f }); // Edge 1-2
		TrianglePointPositions.Add(FMCTriangle{ 2, 3, (PointPositions[2] + PointPositions[3]) * 0.5f }); // Edge 2-3
		TrianglePointPositions.Add(FMCTriangle{ 3, 0, (PointPositions[3] + PointPositions[0]) * 0.5f }); // Edge 3-0

		TrianglePointPositions.Add(FMCTriangle{ 4, 5, (PointPositions[4] + PointPositions[5]) * 0.5f }); // Edge 4-5
		TrianglePointPositions.Add(FMCTriangle{ 5, 6, (PointPositions[5] + PointPositions[6]) * 0.5f }); // Edge 5-6
		TrianglePointPositions.Add(FMCTriangle{ 6, 7, (PointPositions[6] + PointPositions[7]) * 0.5f }); // Edge 6-7
		TrianglePointPositions.Add(FMCTriangle{ 7, 4, (PointPositions[7] + PointPositions[4]) * 0.5f }); // Edge 7-4

		TrianglePointPositions.Add(FMCTriangle{ 0, 4, (PointPositions[0] + PointPositions[4]) * 0.5f }); // Edge 0-4
		TrianglePointPositions.Add(FMCTriangle{ 1, 5, (PointPositions[1] + PointPositions[5]) * 0.5f }); // Edge 1-5
		TrianglePointPositions.Add(FMCTriangle{ 2, 6, (PointPositions[2] + PointPositions[6]) * 0.5f }); // Edge 2-6
		TrianglePointPositions.Add(FMCTriangle{ 3, 7, (PointPositions[3] + PointPositions[7]) * 0.5f }); // Edge 3-7


		PointValues.Init(0, 8);
		BiomeValues.Init(0, 8);
	}

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> PointPositions;	
	UPROPERTY(BlueprintReadWrite)
	TArray<FMCTriangle> TrianglePointPositions;
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> OriginalTrianglePointPositions;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> PointValues;	
	UPROPERTY(BlueprintReadWrite)
	TArray<float> BiomeValues;
	UPROPERTY(BlueprintReadWrite)
	FVector CubeLocation;		
	UPROPERTY(BlueprintReadWrite)
	float CubeSize;

	int LayerNr{ 0 };

	void Update() {
		//function to lerp between 0, 1 of the corners (shoudl give smoother results)
		for (size_t i = 0; i < TrianglePointPositions.Num(); i++)
		{
			float val1{ (-PointValues[TrianglePointPositions[i].pointOne] / 255.0f) };
			float val2{ (PointValues[TrianglePointPositions[i].pointTwo] / 255.0f) };

			FVector Point1{ PointValues[TrianglePointPositions[i].pointOne] };
			FVector Point2{ PointValues[TrianglePointPositions[i].pointTwo] };
			FVector Point3{ TrianglePointPositions[i].OriginalPosition };

			float result{ val1 + val2 };
			float distanceToMove{ (CubeSize * result) };

			if (Point1.X != Point2.X || Point2.X != Point3.X)
				TrianglePointPositions[i].Position.X = TrianglePointPositions[i].OriginalPosition.X + distanceToMove;
			else if (Point1.Y != Point2.Y || Point2.Y != Point3.Y)
				TrianglePointPositions[i].Position.Y = TrianglePointPositions[i].OriginalPosition.Y + distanceToMove;
			else
				TrianglePointPositions[i].Position.Z = TrianglePointPositions[i].OriginalPosition.Z + distanceToMove;
		}
	}
};

USTRUCT(BlueprintType)
struct FMCData {

	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Locations;	

	UPROPERTY(BlueprintReadWrite)
	TArray<int> Values;
};
#pragma endregion

UCLASS()
class CAVEGENRATION_API ANoiseGenerator : public AActor
{
	GENERATED_BODY()

public:

	ANoiseGenerator();

	#pragma region comps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	UALG_CellularAutomata* CellularAutomataComponent{ nullptr };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	UALG_MetaBall* MetaBallsComponent{ nullptr };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	UALG_MarchingCubes* MarchingCubesComponent{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	EAlgorithms SelectedAlgorithm{ EAlgorithms::CELLULAR_AUTOMATA };
	#pragma endregion

	#pragma region settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	int MinAirValue{ 180 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	int MaxAirValue{ 60 }; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float DefaultTimer{ 1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float PerlinScale{ 0.1f };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ZOffset{ 1000 };
	//TODO add a var like MinAirValue, but for the ground level part of a biome (ground goes up until 2 points above) keep into account the nr of layers (this means multiple floors/ceilings)
	//TODO add a var like MaxAirValue, but for the ceiling level part of a biome (ground goes up until 2 points above)
	// OR calculate the faces that look down to generate ceiling stuff, and floor stuff for faces looking up (forward vector of a triangle?).
	#pragma endregion

	#pragma region grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	FVector CubeGridSize{ 20,20,20 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	float CubeSize = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool ShowOldGrid{ true };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool CanDrawGrid{ true };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool DrawDebugAsBox{ true };		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool CanDrawBoundaries{ true };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool DrawAllPoints{ false };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool DrawBiomePoints{ false };
	#pragma endregion

	virtual void Tick(float DeltaTime) override;

	#pragma region ufunctions
	UFUNCTION(BlueprintCallable)
	void RegenerateGrid();
	UFUNCTION(BlueprintCallable)
	FString CycleAlgorithm();	
	UFUNCTION(BlueprintCallable)
	bool GetIsGenerated() { return Task.IsValid() && Task.GetReference()->IsComplete(); };
	UFUNCTION(BlueprintCallable)
	void SetCubeGridX(int value) { CubeGridSize.X = value; GridSize.X = value * 2; };
	UFUNCTION(BlueprintCallable)
	void SetCubeGridY(int value) { CubeGridSize.Y = value; GridSize.Y = value * 2; };
	UFUNCTION(BlueprintCallable)
	void SetCubeGridZ(int value) { CubeGridSize.Z = value; GridSize.Z = value * 2; };
	#pragma endregion

	void HandleComponentChange();
	void CreateCubeGrid();
	bool CheckValues();
	float RemapValue(float Value, int OldMin, int OldMax, int NewMin, int NewMax);
	void HandlePreMeshGen();

	#pragma region vars
	FCriticalSection CriticalSection;
	const int white{ 255 };
	const int black{ 0 };
	FVector GridSize{ 40, 40, 40 };
	FMCData McData{  };

	TMultiMap<FVector, FMCCubeIndex> vertTable{};
	TMap<FVector, FMCCube> GridCubes{ };	
	TMap<FVector, FMCCube> GridCubesForMesh{ };	

	ABiomeGenerator* BiomeGenerator{ nullptr };
	#pragma endregion

protected:
	virtual void BeginPlay() override;

	void CreateGrid();

private:	
	float Offset = 40;

	FVector MaxBoundary{};
	FVector MinBoundary{};
	EAlgorithms PrevAlgorithm{ EAlgorithms::LSYSTEMS };
	UALG_Base* SelectedComponent{ nullptr };

	float timer{ 1 };
	FGraphEventRef Task{};

	void Draw();

};

class FNoiseGeneratorWorker2 : public FRunnable
{
public:
	FNoiseGeneratorWorker2(ANoiseGenerator* noiseGenerator, ABiomeGenerator* biomeGenerator) :
		NoiseGenerator{ noiseGenerator },
		BiomeGenerator{ biomeGenerator }
	{}

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override
	{
		BiomeGenerator->InitNoise();
		BiomeGenerator->FillColorArr();

		NoiseGenerator->GridCubesForMesh.Empty();
		TArray<FVector> keys;
		NoiseGenerator->vertTable.GetKeys(keys);
		while (!NoiseGenerator->vertTable.IsEmpty())
		{
			TArray<FMCCubeIndex> OutValues;
			NoiseGenerator->vertTable.MultiFind(keys[0], OutValues);

			//Perlin noise for cave gen
			FVector PointPosition{ keys[0] };
			PointPosition *= NoiseGenerator->PerlinScale;
			const float PerlinValue{ FMath::PerlinNoise3D(PointPosition) };
			const int State{ FMath::RoundToInt(((PerlinValue + 1) / 2) * 255) };

			//Other noise for biome gen
			const int biomeState{ BiomeGenerator->GetBiomeNoise(PointPosition) };

			bool canDelete{ false };
			for (const FMCCubeIndex& Entry : OutValues)
			{
				if (NoiseGenerator->GridCubes.IsEmpty()) break;
				if (NoiseGenerator->GridCubes.Contains(Entry.cubeLocation)) {
					canDelete = true;

					//Point values for mesh gen
					if (State > NoiseGenerator->MinAirValue && State < NoiseGenerator->MaxAirValue) {
						NoiseGenerator->GridCubes[Entry.cubeLocation].PointValues[Entry.vertIndex] = white;
					}
					else {
						NoiseGenerator->GridCubes[Entry.cubeLocation].PointValues[Entry.vertIndex] = black;
					}
					//NoiseGenerator->GridCubes[Entry.cubeLocation].PointValues[Entry.vertIndex] = State;

					//Biome values for biome gen
					NoiseGenerator->GridCubes[Entry.cubeLocation].BiomeValues[Entry.vertIndex] = BiomeGenerator->FindClosestFloat(biomeState);
				}

			}

			if (canDelete) {
				NoiseGenerator->vertTable.Remove(keys[0]); 
				keys.RemoveAt(0);
			}
		}

		//TArray<FVector> keys2;
		//NoiseGenerator->GridCubes.GetKeys(keys2);
		//for (const auto& key : keys2)
		//{
		//	NoiseGenerator->GridCubes[key].Update();
		//}
		return 0;
	}

	virtual void Stop() override {}

private:
	ANoiseGenerator* NoiseGenerator;
	ABiomeGenerator* BiomeGenerator;
	int white{ 255 };
	int black{ 0 };
};