// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <cavegenration/noise/Noise_Perlin.h>
#include <cavegenration/noise/Noise_Simplex.h>
#include <cavegenration/Algorithms/ALG_CellularAutomata.h>
#include <cavegenration/GameEnums.h>
#include "A_NoiseVisualiser.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <cavegenration/Algorithms/ALG_MetaBall.h>
#include <cavegenration/Algorithms/ALG_MarchingCubes.h>
#include <ProceduralMeshComponent.h>
#include "NoiseGenerator.generated.h"


USTRUCT(BlueprintType)
struct FMCCubeIndex {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector vertLocation;	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int cubeIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int vertIndex;
};

USTRUCT(BlueprintType)
struct FMCCube {
	GENERATED_BODY()

	FMCCube() {};

	FMCCube(FVector location, float cubeSize) {
		CubeLocation = location;

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
		TrianglePointPositions.Add((PointPositions[0] + PointPositions[1]) * 0.5f); // Edge 0-1
		TrianglePointPositions.Add((PointPositions[1] + PointPositions[2]) * 0.5f); // Edge 1-2
		TrianglePointPositions.Add((PointPositions[2] + PointPositions[3]) * 0.5f); // Edge 2-3
		TrianglePointPositions.Add((PointPositions[3] + PointPositions[0]) * 0.5f); // Edge 3-0

		TrianglePointPositions.Add((PointPositions[4] + PointPositions[5]) * 0.5f); // Edge 4-5
		TrianglePointPositions.Add((PointPositions[5] + PointPositions[6]) * 0.5f); // Edge 5-6
		TrianglePointPositions.Add((PointPositions[6] + PointPositions[7]) * 0.5f); // Edge 6-7
		TrianglePointPositions.Add((PointPositions[7] + PointPositions[4]) * 0.5f); // Edge 7-4

		TrianglePointPositions.Add((PointPositions[0] + PointPositions[4]) * 0.5f); // Edge 0-4
		TrianglePointPositions.Add((PointPositions[1] + PointPositions[5]) * 0.5f); // Edge 1-5
		TrianglePointPositions.Add((PointPositions[2] + PointPositions[6]) * 0.5f); // Edge 2-6
		TrianglePointPositions.Add((PointPositions[3] + PointPositions[7]) * 0.5f); // Edge 3-7

		PointValues.Init(0, 8);
		//PointValues[0] = 255;
		//PointValues[1] = 255;
		//PointValues[2] = 255;

		////PointValues[5] = 255;
		//PointValues[7] = 255;
	}

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> PointPositions;	
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> TrianglePointPositions;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> PointValues;
	UPROPERTY(BlueprintReadWrite)
	FVector CubeLocation;
};

USTRUCT(BlueprintType)
struct FCornerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<float> CornerValues;
};

USTRUCT(BlueprintType)
struct FCubeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> CubeLocation;
};

USTRUCT(BlueprintType)
struct FMCData {

	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Locations;

	UPROPERTY(BlueprintReadWrite)
	TArray<int> Values;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCornerData> Corners{};

	UPROPERTY(BlueprintReadWrite)
	TArray<FCubeData> Cubes{};
};

UCLASS()
class CAVEGENRATION_API ANoiseGenerator : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	ANoiseGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	UALG_CellularAutomata* CellularAutomataComponent{ nullptr };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	UALG_MetaBall* MetaBallsComponent{ nullptr };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	UALG_MarchingCubes* MarchingCubesComponent{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Algorithm)
	EAlgorithms SelectedAlgorithm{ EAlgorithms::CELLULAR_AUTOMATA };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	int ColorToWatch{ 30 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float DefaultTimer{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UProceduralMeshComponent* ProcMesh{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UMaterialInstance* ProcMeshMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	FVector CubeGridSize{ 20,20,20 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	float CubeSize = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	TArray<FMCCube> GridCubes{ };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	TArray<FMCCubeIndex> vertTable{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool ShowOldGrid{ true };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool DrawDebugAsBox{ true };	

	FMCData McData{  };
	int white{ 255 };
	int black{ 0 };
	FVector GridSize{ 40, 40, 40 };
	float Offset = 40;

	virtual void Tick(float DeltaTime) override;

	void HandleComponentChange();

	UFUNCTION(BlueprintCallable)
	void GenerateMesh();
	UFUNCTION(BlueprintCallable)
	void RegenerateGrid();
	UFUNCTION(BlueprintCallable)
	FString CycleAlgorithm();	
	UFUNCTION(BlueprintCallable)
	void ClearMesh() { ProcMesh->ClearAllMeshSections(); };

	void CreateCubeGrid();
	bool CheckValues();

	FCriticalSection CriticalSection;

protected:
	virtual void BeginPlay() override;


	void CreateGrid();

private:	

	FVector MaxBoundary{};
	FVector MinBoundary{};
	EAlgorithms PrevAlgorithm{ EAlgorithms::LSYSTEMS };
	UALG_Base* SelectedComponent{ nullptr };

	float timer{ 1 };
	bool IsMulticolor = false;

	void Draw();
	TArray<int> FindSharedVertices(int i, int j, int k, const FVector& GridSize);


};

class FNoiseGeneratorWorker : public FRunnable
{
public:
	FNoiseGeneratorWorker(ANoiseGenerator* InNoiseGenerator, int32 InX, int32 InY)
		: NoiseGenerator(InNoiseGenerator), X(InX), Y(InY)
	{
	}

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override
	{
		if (NoiseGenerator)
		{
			for (int k = 0; k < NoiseGenerator->CubeGridSize.Z; k++)
			{
				int Index = X + Y * NoiseGenerator->CubeGridSize.X + k * NoiseGenerator->CubeGridSize.X * NoiseGenerator->CubeGridSize.Y;
				FVector SpawnLocation = FVector{ X * (NoiseGenerator->CubeSize * 2), Y * (NoiseGenerator->CubeSize * 2), k * (NoiseGenerator->CubeSize * 2) };

				FMCCube Cube(SpawnLocation, NoiseGenerator->CubeSize);
				for (int G = 0; G < Cube.PointPositions.Num(); G++)
				{
					FScopeLock Lock(&NoiseGenerator->CriticalSection);
					NoiseGenerator->vertTable.Add(FMCCubeIndex{ Cube.PointPositions[G], Index, G });
				}
				NoiseGenerator->GridCubes.Add(Cube);
			}
		}

		return 0;
	}

	virtual void Exit() override
	{
	}

private:
	ANoiseGenerator* NoiseGenerator;
	int32 X, Y;
};

class FNoiseGeneratorWorker2 : public FRunnable
{
public:
	FNoiseGeneratorWorker2(TArray<FMCCubeIndex>& InVertTable, TArray<FMCCube>& InGridCubes, bool iMulticolor, int colorToWatch) : 
		VertTable(InVertTable),
		GridCubes(InGridCubes),
		IsMulticolor{IsMulticolor},
		ColorToWatch{ColorToWatch}
	{}

	virtual bool Init() override
	{
		// Initialize any worker-specific data here
		return true;
	}

	virtual uint32 Run() override
	{
		// Your multithreaded logic goes here
		while (!VertTable.IsEmpty())
		{
			int i2 = 0;
			for (const auto& Pair : VertTable)
			{
				TArray<FMCCubeIndex> OutValues;

				for (const auto& Pair2 : VertTable)
				{
					if (Pair2.vertLocation == Pair.vertLocation)
					{
						OutValues.Add(Pair2);
					}
				}

				int State = FMath::RandRange(black, white);
				for (size_t i = 0; i < OutValues.Num(); i++)
				{
					if (IsMulticolor)
					{
						GridCubes[OutValues[i].cubeIndex].PointValues[OutValues[i].vertIndex] = State;
					}
					else
					{
						GridCubes[OutValues[i].cubeIndex].PointValues[OutValues[i].vertIndex] = State < ColorToWatch ? white : black;
					}
					VertTable.RemoveAt(i2);
					i2++;
				}
				break;
			}
		}

		return 0;
	}

	virtual void Stop() override
	{
		// Clean up any resources here
	}

private:
	TArray<FMCCubeIndex>& VertTable;
	TArray<FMCCube>& GridCubes;
	int white{ 255 };
	int black{ 0 };
	bool IsMulticolor{ false };
	int ColorToWatch{ 0 };
};