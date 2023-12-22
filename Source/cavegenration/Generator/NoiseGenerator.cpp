// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseGenerator.h"

struct FCustomInstanceData {
	FColor Color{};
};

// Sets default values
ANoiseGenerator::ANoiseGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
	CellularAutomataComponent = CreateDefaultSubobject<UALG_CellularAutomata>(TEXT("CellularAutomataComponent"));
	MetaBallsComponent = CreateDefaultSubobject<UALG_MetaBall>(TEXT("MetaBallsComponent"));
	MarchingCubesComponent = CreateDefaultSubobject<UALG_MarchingCubes>(TEXT("MarchingCubesComponent"));

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	RootComponent = ProcMesh;
	ProcMesh->bUseAsyncCooking = true;

}

// Called when the game starts or when spawned
void ANoiseGenerator::BeginPlay()
{
	Super::BeginPlay();

	GridSize = CubeGridSize * 2;
	Offset = CubeSize / 2;

	DefaultTimer = 0.3;

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
		{
			CreateGrid();
			CreateCubeGrid();

			CycleAlgorithm();
		});
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
		//IsMulticolor = true;
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

	//InitValues();
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

		const FVector pos = FVector(i * Offset, j * Offset, k * Offset);

		FScopeLock Lock(&CriticalSection);
		McData.Locations.Add(pos);
		const int State{ IsMulticolor ? FMath::Rand() : FMath::RandBool() ? white : black };
		McData.Values.Add(State);
	});

	if (TotalSize > 0)
	{
		MinBoundary = McData.Locations[0];
		MaxBoundary = McData.Locations[TotalSize - 1];
	}
}

void ANoiseGenerator::CreateCubeGrid()
{
	GridCubes.Empty();
	vertTable.Empty();

	ParallelFor(CubeGridSize.X * CubeGridSize.Y * CubeGridSize.Z, [&](int32 Index)
	{
		int i, j, k;
		{
			FScopeLock Lock(&CriticalSection);
			i = Index % int(CubeGridSize.X);
			j = (Index / int(CubeGridSize.X)) % int(CubeGridSize.Y);
			k = Index / int(CubeGridSize.X * CubeGridSize.Y);
		}

		const FVector SpawnLocation = FVector{ i * (CubeSize * 2), j * (CubeSize * 2), k * (CubeSize * 2) };
		const auto cube = FMCCube(SpawnLocation, CubeSize);
		{
			FScopeLock Lock(&CriticalSection);
			for (int g = 0; g < cube.PointPositions.Num(); g++)
			{
				vertTable.Add(cube.PointPositions[g], FMCCubeIndex{ cube.PointPositions[g], Index, g });
			}

			GridCubes.Add(cube);
		}
	});


	MinBoundary = GridCubes[0].CubeLocation;
	MaxBoundary = GridCubes[GridCubes.Num() - 1].CubeLocation;
}

bool ANoiseGenerator::CheckValues()
{
	FNoiseGeneratorWorker2* Worker = new FNoiseGeneratorWorker2(this, IsMulticolor, ColorToWatch);
	FRunnableThread* Thread = FRunnableThread::Create(Worker, TEXT("NoiseGeneratorThread2"), 0, TPri_AboveNormal);
	Thread->WaitForCompletion();

	delete Thread;
	delete Worker;

	return true;
}

// Called every frame
void ANoiseGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timer -= DeltaTime;
	if (PrevAlgorithm != SelectedAlgorithm) {
		PrevAlgorithm = SelectedAlgorithm;

		HandleComponentChange();
	}

	//Update();
	if (!Task.IsValid()) {
		if (SelectedComponent) {
			SelectedComponent->Update();
		}
		Draw();
	}
	else {
		if (Task.GetReference()->IsComplete()) {
			if (SelectedComponent) {
				SelectedComponent->Update();
			}
			Draw();
		}
	}
}

void ANoiseGenerator::Draw()
{
	if (!CanDraw) return;
	if (ShowOldGrid) {
		int i{ 0 };
		for (const auto value : McData.Values) {
			const uint8 color{ static_cast<uint8>(value) };
			DrawDebugPoint(GetWorld(), McData.Locations[i], 4, FColor{ color, color, color });
			i++;
		}
	}
	else {
		for (const FMCCube& cube : GridCubes)
		{
			if (DrawDebugAsBox) {
				DrawDebugBox(GetWorld(), cube.CubeLocation, FVector{ CubeSize }, FColor::Red);
			}
			else {
				const auto positions{ cube.PointPositions };
				for (size_t g = 0; g < positions.Num(); g++)
				{
					const uint8 color{ static_cast<uint8>(cube.PointValues[g])};
					DrawDebugPoint(GetWorld(), positions[g], 4, FColor{color, color, color});

					FString txt{ };
					txt.AppendInt(g);
					DrawDebugString(GetWorld(), positions[g], txt, this, FColor::Red);
				}
			}
		}
	}
}

#pragma region UI
void ANoiseGenerator::GenerateMesh() {
	if (SelectedComponent) {
		SelectedComponent->GenerateMesh();
	}
}

void ANoiseGenerator::RegenerateGrid()
{
	
	Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&]()
		{
		if (SelectedAlgorithm == EAlgorithms::CELLULAR_AUTOMATA) {
			CreateGrid();
		}
		else {
			CreateCubeGrid();
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
