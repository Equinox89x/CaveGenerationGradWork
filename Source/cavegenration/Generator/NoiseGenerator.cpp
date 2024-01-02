// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseGenerator.h"
#include <Kismet/GameplayStatics.h>

ANoiseGenerator::ANoiseGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	CellularAutomataComponent = CreateDefaultSubobject<UALG_CellularAutomata>(TEXT("CellularAutomataComponent"));
	MetaBallsComponent = CreateDefaultSubobject<UALG_MetaBall>(TEXT("MetaBallsComponent"));
	MarchingCubesComponent = CreateDefaultSubobject<UALG_MarchingCubes>(TEXT("MarchingCubesComponent"));
}

void ANoiseGenerator::BeginPlay()
{
	Super::BeginPlay();

	BiomeGenerator = Cast<ABiomeGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ABiomeGenerator::StaticClass()));

	GridSize = CubeGridSize * 2;
	Offset = CubeSize / 2;

	DefaultTimer = 0.3;

	RegenerateGrid();
}

void ANoiseGenerator::HandleComponentChange()
{
	switch (SelectedAlgorithm)
	{
	case EAlgorithms::CELLULAR_AUTOMATA:
		MetaBallsComponent->Cleanup();
		MarchingCubesComponent->Cleanup();

		SelectedComponent = CellularAutomataComponent;
		break;
	case EAlgorithms::META_BALLS:
		MarchingCubesComponent->Cleanup();

		SelectedComponent = MetaBallsComponent;
		break;
	case EAlgorithms::MARCHING_CUBES:
		MetaBallsComponent->Cleanup();

		SelectedComponent = MarchingCubesComponent;
		break;
	case EAlgorithms::VORONOI:
		break;
	case EAlgorithms::RANDOM_WALK:
		break;
	case EAlgorithms::LSYSTEMS:
		break;
	default:
		break;
	}

	if (SelectedComponent) {
		SelectedComponent->InitValues(MinBoundary, MaxBoundary);
	}
}

void ANoiseGenerator::CreateGrid()
{
	McData.Locations.Empty();
	McData.Values.Empty();

	const int TotalSize{ int(GridSize.X) * int(GridSize.Y) * int(GridSize.Z) };
	McData.Locations.Reserve(TotalSize);
	McData.Values.Reserve(TotalSize);

	ParallelFor(TotalSize, [this](int Index)
	{
		const int i = Index % int(GridSize.X);
		const int j = (Index / int(GridSize.X)) % int(GridSize.Y);
		const int k = Index / int(GridSize.X * GridSize.Y);

		FVector pos = FVector(i * Offset, j * Offset, k * Offset);

		FScopeLock Lock(&CriticalSection);
		McData.Locations.Add(pos);
		pos *= PerlinScale;
		const float PerlinValue = FMath::PerlinNoise3D(pos);
		const int State{ FMath::RoundToInt(((PerlinValue + 1) / 2) * 255) };
		if (State < BiomeGenerator->Treshold) {
			McData.Values.Add(white);
		}
		else {
			McData.Values.Add(black);
		}
		//McData.Values.Add(State);
	});

	//for (int x = 0; x < GridSize.X; ++x)
	//{
	//	for (int y = 0; y < GridSize.Y; ++y)
	//	{
	//		for (int z = 0; z < GridSize.Z; ++z)
	//		{
	//			TArray<int> indexes{
	//				GetIndex(x, y, z),
	//				GetIndex(x + 1, y, z),
	//				GetIndex(x, y + 1, z),
	//				GetIndex(x + 1, y + 1, z),
	//				GetIndex(x, y, z + 1),
	//				GetIndex(x + 1, y, z + 1),
	//				GetIndex(x, y + 1, z + 1),
	//				GetIndex(x + 1, y + 1, z + 1),
	//			};

	//			TArray<float> values;
	//			for (const auto& index : indexes)
	//			{
	//				if (McData.Values.Num() > index) {
	//					values.Add(McData.Values[index]);
	//				}
	//			}

	//			// Calculate the 8-bit code representing the point's configuration
	//			int PointConfigIndex{ 0 };
	//			for (int j = 0; j < values.Num(); ++j)
	//			{
	//				if (values[j] > 0.5f)
	//				{
	//					PointConfigIndex |= 1 << j;
	//				}
	//			}
	//			McData.configs.Add(PointConfigIndex);
	//		}
	//	}
	//}

}

void ANoiseGenerator::CreateCubeGrid()
{
	auto size{ CubeGridSize.X * CubeGridSize.Y * CubeGridSize.Z };
	GridCubes.Empty();
	vertTable.Empty();
	GridCubes.Reserve(size);
	vertTable.Reserve(size * 3);

	ParallelFor(CubeGridSize.X * CubeGridSize.Y * CubeGridSize.Z, [&](int32 Index)
		{
			int i, j, k;
			{
				FScopeLock Lock(&CriticalSection);
				i = Index % int(CubeGridSize.X);
				j = (Index / int(CubeGridSize.X)) % int(CubeGridSize.Y);
				k = Index / int(CubeGridSize.X * CubeGridSize.Y);
			}

			const FVector spawnLocation = FVector{ i * (CubeSize * 2), j * (CubeSize * 2), k * (CubeSize * 2) };
			const auto cube = FMCCube(spawnLocation, CubeSize, Index);
			{
				FScopeLock Lock(&CriticalSection);
				for (int g = 0; g < cube.PointPositions.Num(); g++)
				{
					vertTable.Add(cube.PointPositions[g], FMCCubeIndex{ cube.PointPositions[g], spawnLocation, g });
				}

				GridCubes.Add(spawnLocation, cube);
			}
		});

	TArray<FVector> keys;
	GridCubes.GetKeys(keys);

	FVector SmallestVector;
	float SmallestMagnitude = TNumericLimits<float>::Max();  // Initialize with the highest possible value

	for (const FVector& CurrentVector : keys)
	{
		float CurrentMagnitude = CurrentVector.Size();

		if (CurrentMagnitude < SmallestMagnitude)
		{
			SmallestMagnitude = CurrentMagnitude;
			SmallestVector = CurrentVector;
		}
	}

	MinBoundary = SmallestVector;

	FVector LargestVector;
	float LargestMagnitude = 0.0f;  // Initialize with the lowest possible magnitude

	for (const FVector& CurrentVector : keys)
	{
		float CurrentMagnitude = CurrentVector.Size();

		if (CurrentMagnitude > LargestMagnitude)
		{
			LargestMagnitude = CurrentMagnitude;
			LargestVector = CurrentVector;
		}
	}

	MaxBoundary = LargestVector;

	BiomeGenerator->InitBiomeLayers(MinBoundary, MaxBoundary);

	GenerateWall();
}

void ANoiseGenerator::GenerateWall()
{
	auto newMin{ MinBoundary };
	newMin -= FVector(CubeSize);
	auto newMax{ MaxBoundary };
	newMax += FVector(CubeSize);
	GridCubesForWall = new FMCWall(newMin, MaxBoundary);

	//FVector FarEndCorners[8];
	//FarEndCorners[0] = FVector(MinBoundary);
	//FarEndCorners[1] = FVector(MinBoundary.X, MaxBoundary.Y, MinBoundary.Z);
	//FarEndCorners[2] = FVector(MaxBoundary.X, MinBoundary.Y, MinBoundary.Z);
	//FarEndCorners[3] = FVector(MaxBoundary.X, MaxBoundary.Y, MinBoundary.Z);
	//
	//FarEndCorners[4] = FVector(MaxBoundary);
	//FarEndCorners[5] = FVector(MinBoundary.X, MaxBoundary.Y, MaxBoundary.Z);
	//FarEndCorners[6] = FVector(MaxBoundary.X, MinBoundary.Y, MaxBoundary.Z);
	//FarEndCorners[7] = FVector(MaxBoundary.X, MaxBoundary.Y, MaxBoundary.Z);

	//TArray<FVector> Vertices;
	//TArray<int32> Triangles;

	//int32 Indices[36] = {
	//	0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 0, 4, 7, 0, 7, 3, 1, 5, 6, 1, 6, 2, 3, 7, 6, 3, 6, 2, 0, 4, 5, 0, 5, 1
	//};

	//for (int i = 0; i < 36; ++i) {
	//	Triangles.Add(Indices[i]);
	//}

	//ProceduralMeshComponent->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	//const int z{ 1 };
	//GridCubesForWall.Empty();
	//GridCubesForWall.Reserve(CubeGridSize.X * CubeGridSize.Y * (z * 2));

	//// top-bottom, front-back, left-right
	//const TArray<TArray<int>> PointValueIndexes{ {4,5,6,7}, {4,5,6,7}, {0,3,4,7},  {0,3,4,7}, {0,1,4,5}, {0,1,4,5} };

	//for (size_t Index = 0; Index < CubeGridSize.X * CubeGridSize.Y * z; Index++)
	//{
	//	int i = Index % int(CubeGridSize.X);
	//	int j = (Index / int(CubeGridSize.X)) % int(CubeGridSize.Y);
	//	int k = Index / int(CubeGridSize.X * CubeGridSize.Y);

	//	FVector spawnLocations[6];
	//	spawnLocations[0] = FVector{ i * (CubeSize * 2), j * (CubeSize * 2), k * (CubeSize * 2) };
	//	spawnLocations[0].Z -= CubeSize;
	//	spawnLocations[1] = FVector{ i * (CubeSize * 2), j * (CubeSize * 2), k * (CubeSize * 2) };
	//	spawnLocations[1].Z += (CubeGridSize.X - 1) * CubeSize;
	//	spawnLocations[2] = FVector{ k * (CubeSize * 2), i * (CubeSize * 2), j * (CubeSize * 2) };
	//	spawnLocations[2].X += (CubeGridSize.X * 2 - 1) * CubeSize;
	//	spawnLocations[3] = FVector{ k * (CubeSize * 2), i * (CubeSize * 2), j * (CubeSize * 2) };
	//	spawnLocations[3].X -= CubeSize;
	//	spawnLocations[4] = FVector{ i * (CubeSize * 2), k * (CubeSize * 2), j * (CubeSize * 2) };
	//	spawnLocations[4].Y -= CubeSize;
	//	spawnLocations[5] = FVector{ i * (CubeSize * 2), k * (CubeSize * 2), j * (CubeSize * 2) };
	//	spawnLocations[5].Y += (CubeGridSize.Y * 2 - 1) * CubeSize;


	//	for (int dir = 0; dir < 6; ++dir)
	//	{
	//		auto cube = FMCCube(spawnLocations[dir], CubeSize, Index);

	//		for (size_t pointIndex = 0; pointIndex < PointValueIndexes[dir].Num(); pointIndex++)
	//		{
	//			cube.PointValues[PointValueIndexes[dir][pointIndex]] = 255;
	//		}
	//		GridCubesForWall.Add(spawnLocations[dir], cube);
	//	}
	//}
}

bool ANoiseGenerator::CheckValues()
{
	FNoiseGeneratorWorker2* Worker = new FNoiseGeneratorWorker2(this, BiomeGenerator);
	FRunnableThread* Thread = FRunnableThread::Create(Worker, TEXT("NoiseGeneratorThread2"), 0, TPri_AboveNormal);
	Thread->WaitForCompletion();

	delete Thread;
	delete Worker;

	return true;
}

float ANoiseGenerator::RemapValue(float Value, int OldMin, int OldMax, int NewMin, int NewMax)
{
	return (Value - OldMin) * (float(NewMax - NewMin) / float(OldMax - OldMin)) + NewMin;
}

void ANoiseGenerator::HandlePreMeshGen()
{
	SelectedComponent->HandlePreMeshGen();
}

void ANoiseGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timer -= DeltaTime;
	if (PrevAlgorithm != SelectedAlgorithm) {
		PrevAlgorithm = SelectedAlgorithm;

		HandleComponentChange();
	}

	if (!Task.IsValid()) {
		if (SelectedComponent) {
			SelectedComponent->Update();
		}
		if (!CanDrawGrid) return;
		Draw();
	}
	else {
		if (Task.GetReference()->IsComplete()) {
			if (SelectedComponent) {
				SelectedComponent->Update();
			}
			if (!CanDrawGrid) return;
			Draw();
		}
	}
}

void ANoiseGenerator::Draw()
{
	if (CanDrawBoundaries) {		
		//DrawDebugBox(GetWorld(), ProcMesh->Bounds.Origin, ProcMesh->Bounds.BoxExtent, FColor::Red);
	}
	if (ShowOldGrid) {
		int i{ 0 };
		for (const auto value : McData.Values) {
			const uint8 color{ static_cast<uint8>(value) };
			DrawDebugPoint(GetWorld(), McData.Locations[i], 4, FColor{ color, color, color });
			i++;
		}
	}
	else {
		for (auto& cube : GridCubes)
		{
			if (DrawDebugAsBox) {
				DrawDebugBox(GetWorld(), cube.Value.CubeLocation, FVector{ CubeSize }, FColor::Red);
			}
			else {
				const auto positions{ cube.Value.PointPositions };
				for (size_t g = 0; g < positions.Num(); g++)
				{
					const uint8 color{ static_cast<uint8>(DrawBiomePoints ? cube.Value.BiomeValues[g] : cube.Value.PointValues[g]) };
					DrawDebugPoint(GetWorld(), positions[g], 4, FColor{color, color, color});
				}

				if (DrawAllPoints) {
					cube.Value.Update();
					const auto positions2{ cube.Value.TrianglePointPositions };
					for (size_t g = 0; g < positions2.Num(); g++)
					{
						DrawDebugPoint(GetWorld(), positions2[g].Position, 4, FColor::Red);
					}
				}
			}
		}
	}
}

#pragma region UI
void ANoiseGenerator::RegenerateGrid()
{
	
	Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]()
	{
		CreateGrid();
		CreateCubeGrid();
		if (SelectedComponent != MetaBallsComponent) {
			CheckValues();
		}
		HandleComponentChange();
	},
	TStatId(), nullptr, ENamedThreads::AnyBackgroundThreadNormalTask);
}

FString ANoiseGenerator::CycleAlgorithm()
{
	const int index = (static_cast<int>(SelectedAlgorithm) + 1) % static_cast<int>(EAlgorithms::VORONOI);
	SelectedAlgorithm = static_cast<EAlgorithms>(index);

	if (SelectedAlgorithm == EAlgorithms::VORONOI) {
		SelectedAlgorithm = EAlgorithms::CELLULAR_AUTOMATA;
	}

	HandleComponentChange();
	FString str{ UEnum::GetValueAsString(SelectedAlgorithm) };
	str.RemoveAt(0, 13);
	return str;
}
#pragma endregion