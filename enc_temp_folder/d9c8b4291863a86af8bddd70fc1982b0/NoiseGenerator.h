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
//#include <ThirdParty/FastNoiseGenerator/Source/FastNoiseGenerator/Public/FastNoiseWrapper.h>
#include "NoiseGenerator.generated.h"


USTRUCT(BlueprintType)
struct FMCCubeIndex {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector vertLocation;	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//int cubeIndex;
	FVector cubeIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int vertIndex;
};

USTRUCT(BlueprintType)
struct FMCCube {
	GENERATED_BODY()

	FMCCube() {};

	FMCCube(FVector location, float cubeSize, int index) {
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

		Index = index;
	}

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> PointPositions;	
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> TrianglePointPositions;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> PointValues;
	UPROPERTY(BlueprintReadWrite)
	FVector CubeLocation;	
	UPROPERTY(BlueprintReadWrite)
	int Index;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float PerlinScale{ 0.1f };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float ZOffset{ 1000 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UProceduralMeshComponent* ProcMesh{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UMaterialInstance* ProcMeshMaterial;	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	//UFastNoiseWrapper* NoiseGen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	FVector CubeGridSize{ 20,20,20 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	float CubeSize = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	//TArray<FMCCube> GridCubes{ };	
	TMap<FVector, FMCCube> GridCubes{ };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool ShowOldGrid{ true };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool CanDraw{ true };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
	bool DrawDebugAsBox{ true };	


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
	UFUNCTION(BlueprintCallable)
	bool GetIsGenerated() { return Task.IsValid() && Task.GetReference()->IsComplete(); };
	UFUNCTION(BlueprintCallable)
	void SetCubeGridX(int value) { CubeGridSize.X = value; GridSize.X = value * 2; };
	UFUNCTION(BlueprintCallable)
	void SetCubeGridY(int value) { CubeGridSize.Y = value; GridSize.Y = value * 2; };
	UFUNCTION(BlueprintCallable)
	void SetCubeGridZ(int value) { CubeGridSize.Z = value; GridSize.Z = value * 2; };

	void CreateCubeGrid();
	bool CheckValues();

	float RemapValue(float Value, int OldMin, int OldMax, int NewMin, int NewMax);


	FCriticalSection CriticalSection;
	const int white{ 255 };
	const int black{ 0 };
	FVector GridSize{ 40, 40, 40 };
	FMCData McData{  };
	TMultiMap<FVector, FMCCubeIndex> vertTable{};

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
	FNoiseGeneratorWorker2(ANoiseGenerator* noiseGenerator) :
		NoiseGenerator{ noiseGenerator }
	{}

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override
	{
		FMath::SRandInit(123);
		//for (const auto& Pair : NoiseGenerator->vertTable)
		//{
		//	TArray<FMCCubeIndex> OutValues;
		//	NoiseGenerator->vertTable.MultiFind(Pair.Key, OutValues);

		//	//int State = FMath::RandRange(black, white);

		//	FVector PointPosition{ Pair.Key };
		//	PointPosition *= NoiseGenerator->PerlinScale;
		//	const float PerlinValue = FMath::PerlinNoise2D(FVector2D{ PointPosition.X, PointPosition.Z });
		//	//const float PerlinValue = NoiseGenerator->NoiseGen->GetNoise3D(static_cast<float>(pos.X), static_cast<float>(pos.Y), static_cast<float>(pos.Z));
		//	//const float normalisedValue{ NoiseGenerator->RemapValue(PerlinValue, -1, 1, black, white) };
		//	const int State{ FMath::RoundToInt(((PerlinValue+1)/2)*255) };

		//	for (const FMCCubeIndex& Entry : OutValues)
		//	{
		//		// TODO Because of parallel execution, the cube index isn't the correct index.
		//		// This is solved by using the cubes location as index and finding the value in the map according to this position, BUT this code can only execute AFTER the full grid was generated.
		//		// Issue: solving this with waiting and etc causes a lot of time where you have to wait considering the vert table has 64k records (nr of cubes * 8).
		//		
		//		// TODO vertIndex 7 always is the bottom right vertex for example. When you have corners of cubes overlapping, they can be for example top right on cube 1 and top left on cube 2.
		//		// These do not have the same vert index, so we will need to use position here to determing which vert we are talking about.

		//		//const int State{ FMath::RoundToInt(PerlinValue * white) };
		//		if (State < NoiseGenerator->ColorToWatch) {
		//			NoiseGenerator->GridCubes[Entry.cubeIndex].PointValues[Entry.vertIndex] = white;
		//		}
		//		else {
		//			NoiseGenerator->GridCubes[Entry.cubeIndex].PointValues[Entry.vertIndex] = black;
		//		}
		//	}
		//}

		TArray<FVector> keys;
		NoiseGenerator->vertTable.GetKeys(keys);
		while (!NoiseGenerator->vertTable.IsEmpty())
		{
			//FMCCubeIndex record = NoiseGenerator->vertTable.Find;
			TArray<FMCCubeIndex> OutValues;
			NoiseGenerator->vertTable.MultiFind(keys[0], OutValues);

			//int State = FMath::RandRange(black, white);

			FVector PointPosition{ keys[0] };
			PointPosition *= NoiseGenerator->PerlinScale;
			const float PerlinValue = FMath::PerlinNoise2D(FVector2D{ PointPosition.X, PointPosition.Z });
			//const float PerlinValue = NoiseGenerator->NoiseGen->GetNoise3D(static_cast<float>(pos.X), static_cast<float>(pos.Y), static_cast<float>(pos.Z));
			//const float normalisedValue{ NoiseGenerator->RemapValue(PerlinValue, -1, 1, black, white) };
			const int State{ FMath::RoundToInt(((PerlinValue + 1) / 2) * 255) };

			bool canDelete{ false };
			for (const FMCCubeIndex& Entry : OutValues)
			{
				// TODO Because of parallel execution, the cube index isn't the correct index.
				// This is solved by using the cubes location as index and finding the value in the map according to this position, BUT this code can only execute AFTER the full grid was generated.
				// Issue: solving this with waiting and etc causes a lot of time where you have to wait considering the vert table has 64k records (nr of cubes * 8).

				// TODO vertIndex 7 always is the bottom right vertex for example. When you have corners of cubes overlapping, they can be for example top right on cube 1 and top left on cube 2.
				// These do not have the same vert index, so we will need to use position here to determing which vert we are talking about.

				//const int State{ FMath::RoundToInt(PerlinValue * white) };
				/*if (NoiseGenerator->GridCubes.Num() < Entry.cubeIndex) break;
				canDelete = true;
				if (State < NoiseGenerator->ColorToWatch) {
					NoiseGenerator->GridCubes[Entry.cubeIndex].PointValues[Entry.vertIndex] = white;
				}
				else {
					NoiseGenerator->GridCubes[Entry.cubeIndex].PointValues[Entry.vertIndex] = black;
				}*/

				if (NoiseGenerator->GridCubes.IsEmpty()) break;
				if (NoiseGenerator->GridCubes.Contains(Entry.cubeIndex)) {
					canDelete = true;
					if (State < NoiseGenerator->ColorToWatch) {
						NoiseGenerator->GridCubes[Entry.cubeIndex].PointValues[Entry.vertIndex] = white;
					}
					else {
						NoiseGenerator->GridCubes[Entry.cubeIndex].PointValues[Entry.vertIndex] = black;
					}
				}
			}

			if (canDelete) {
				NoiseGenerator->vertTable.Remove(keys[0]); 
				keys.RemoveAt(0);
			}
		}

		return 0;
	}

	virtual void Stop() override {}

private:
	ANoiseGenerator* NoiseGenerator;
	int white{ 255 };
	int black{ 0 };
};