// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseGenerator.h"

struct FCustomInstanceData {
	FColor Color{};
};

// Sets default values
ANoiseGenerator::ANoiseGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	MinBoundary = GetActorLocation();
	MaxBoundary = GetActorLocation();
	CreateGrid();
	CreateCubeGrid();

	HandleComponentChange();
}


void ANoiseGenerator::HandleComponentChange()
{
	IsMulticolor =  false;

	switch (SelectedAlgorithm)
	{
	case EAlgorithms::CELLULAR_AUTOMATA:
		MetaBallsComponent->Cleanup();
		MarchingCubesComponent->Cleanup();

		SelectedComponent = CellularAutomataComponent;
		break;
	case EAlgorithms::META_BALLS:
		IsMulticolor = true;
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

	FVector pos{ 0 };
	int index{ 0 };
	for (size_t i = 0; i < GridSize.X; i++)
	{
		for (size_t j = 0; j < GridSize.Y; j++)
		{
			for (size_t k = 0; k < GridSize.Z; k++)
			{
				McData.Locations.Add(pos);

				int Index = i + j * GridSize.X + k * GridSize.X * GridSize.Y;
				int State = FMath::RandRange(black, white);

				if (IsMulticolor)
				{
					McData.Values.Add(State);
				}
				else
				{
					McData.Values.Add(State < ColorToWatch ? white : black);
				}

				pos.Z += Offset;
				index++;
			}

			pos.Z = 0;
			pos.X += Offset;
		}

		pos.X = 0;
		pos.Y += Offset;

		MaxBoundary.X += (Offset);
		MaxBoundary.Y += (Offset);
		MaxBoundary.Z += (Offset);
	}
}

void ANoiseGenerator::CreateCubeGrid()
{
	GridCubes.Empty();
	vertTable.Empty();

	MaxBoundary.X -= 100;
	MaxBoundary.Y -= 100;
	MaxBoundary.Z -= 100;

	TArray<FRunnableThread*> Threads;
	for (int i = 0; i < CubeGridSize.X; i++)
	{
		for (int j = 0; j < CubeGridSize.Y; j++)
		{
			/*for (int k = 0; k < CubeGridSize.Z; k++)
			{*/
				////FNoiseGeneratorWorker* Worker = new FNoiseGeneratorWorker((this, i, j, k));

				//	int Index = i + j * CubeGridSize.X + k * CubeGridSize.X * CubeGridSize.Y;
				//	FVector SpawnLocation = FVector{ i * (CubeSize*2), j * (CubeSize*2), k * (CubeSize *2)};

				//	auto cube = FMCCube(SpawnLocation, CubeSize);
				//	for (int g = 0; g < cube.PointPositions.Num(); g++)
				//	{
				////		FScopeLock Lock(&CriticalSection);
				//		vertTable.Add(FMCCubeIndex{ cube.PointPositions[g], Index, g});
				//	}
				//	GridCubes.Add(cube);
				////});

				FNoiseGeneratorWorker* Worker = new FNoiseGeneratorWorker(this, i, j);
				FRunnableThread* Thread = FRunnableThread::Create(Worker, TEXT("NoiseGeneratorThread"), 0, TPri_Normal);
				Threads.Add(Thread);

			//}
		}

		MaxBoundary.X += CubeSize;
		MaxBoundary.Y += CubeSize;
		MaxBoundary.Z += CubeSize;
	}

	// Wait for all threads to finish
	for (FRunnableThread* Thread : Threads)
	{
		if (Thread)
		{
			Thread->WaitForCompletion();
			delete Thread;
		}
	}
}

bool ANoiseGenerator::CheckValues()
{
	/*while (!vertTable.IsEmpty())
	{
		int i2 = 0;
		for (const auto& Pair : vertTable)
		{
			TArray<FMCCubeIndex> OutValues;

			for (const auto& Pair2 : vertTable)
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
				vertTable.RemoveAt(i2);
				i2++;
			}
			break;
		}
	}

	return true;*/

	FNoiseGeneratorWorker2* Worker = new FNoiseGeneratorWorker2(vertTable, GridCubes, IsMulticolor, ColorToWatch);

	// Create a thread and launch it
	FRunnableThread* Thread = FRunnableThread::Create(Worker, TEXT("NoiseGeneratorThread2"), 0, TPri_AboveNormal);

	// Wait for the thread to finish
	Thread->WaitForCompletion();

	delete Thread;
	delete Worker;

	return true;
}

//bool ANoiseGenerator::CheckValuesRecursive()
//{
//	while (!vertTable.IsEmpty())
//	{
//		int State = FMath::RandRange(black, white);
//
//		TArray<int32> IndicesToRemove;
//
//		for (int i = 0; i < vertTable.Num(); ++i)
//		{
//			const FMCCubeIndex& currentPair = vertTable[i];
//
//			// Find all matching vertLocations
//			TArray<int32> MatchingIndices;
//			for (int j = i + 1; j < vertTable.Num(); ++j)
//			{
//				if (vertTable[j].vertLocation == currentPair.vertLocation)
//				{
//					MatchingIndices.Add(j);
//				}
//			}
//
//			// Set PointValues and mark indices for removal
//			GridCubes[currentPair.cubeIndex].PointValues[currentPair.vertIndex] =
//				IsMulticolor ? State : (State < ColorToWatch ? white : black);
//
//			for (int32 MatchingIndex : MatchingIndices)
//			{
//				IndicesToRemove.Add(MatchingIndex);
//			}
//
//			IndicesToRemove.Add(i);
//		}
//
//		// Remove marked indices in reverse order to avoid invalidating subsequent indices
//		IndicesToRemove.Sort([](int32 A, int32 B) { return A > B; });
//
//		for (int32 IndexToRemove : IndicesToRemove)
//		{
//			vertTable.RemoveAt(IndexToRemove);
//		}
//	}
//
//	return true;
//}

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
	if (SelectedComponent) {
		SelectedComponent->Update();
	}
	Draw();
}

//void ANoiseGenerator::Update()
//{
//	if (timer < 0) {
//		if (ShowOldGrid) {
//			for (int i = 0; i < GridSize.X; ++i)
//			{
//				for (int j = 0; j < GridSize.Y; ++j)
//				{
//					for (int k = 0; k < GridSize.Z; ++k)
//					{
//						int Index = i + j * GridSize.X + k * GridSize.X * GridSize.Y;
//						//auto noise = FMath::PerlinNoise3D(NoiseGenerator->Locations[Index]);
//						//auto convertedValue{ MapValueToRange(noise, MinValue, MaxValue, black, white) };
//						auto convertedValue{ FMath::RandRange(black, white) };
//						McData.Values[Index] = convertedValue < ColorToWatch ? white : black;
//					}
//				}
//			}
//		}
//		timer = DefaultTimer;
//	}
//}

void ANoiseGenerator::Draw()
{
	if (ShowOldGrid) {
		for (int i = 0; i < GridSize.X; ++i)
		{
			for (int j = 0; j < GridSize.Y; ++j)
			{
				for (int k = 0; k < GridSize.Z; ++k)
				{
					int Index = i + j * GridSize.X + k * GridSize.X * GridSize.Y;
					uint8 color{ static_cast<uint8>(McData.Values[Index]) };
					DrawDebugPoint(GetWorld(), McData.Locations[Index], 4, FColor{ color, color, color });
				}
			}
		}
	}
	else {
		for (const FMCCube& cube : GridCubes)
		{
			if (DrawDebugAsBox) {
				DrawDebugBox(GetWorld(), cube.CubeLocation, FVector{ CubeSize }, FColor::Red);
			}
			else {
				auto positions{ cube.PointPositions };
				for (size_t g = 0; g < positions.Num(); g++)
				{
					uint8 color{ static_cast<uint8>(cube.PointValues[g])};
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
	if (SelectedAlgorithm == EAlgorithms::CELLULAR_AUTOMATA) {
		CreateGrid();
	}
	else {
		CreateCubeGrid();
		CheckValues();
	}

	HandleComponentChange();
}

FString ANoiseGenerator::CycleAlgorithm()
{
	int index = (static_cast<int>(SelectedAlgorithm) + 1) % static_cast<int>(EAlgorithms::VORONOI);
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
