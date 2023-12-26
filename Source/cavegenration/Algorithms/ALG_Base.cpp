// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_Base.h"
#include <cavegenration/Generator/NoiseGenerator.h>

// Sets default values for this component's properties
UALG_Base::UALG_Base()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UALG_Base::BeginPlay()
{
	Super::BeginPlay();

	NoiseGenerator = Cast<ANoiseGenerator>(GetOwner());
}

void UALG_Base::Update() {
	timer -= deltatime;
}

void UALG_Base::Draw()
{
	for (int i = 0; i < NoiseGenerator->GridSize.X; ++i)
	{
		for (int j = 0; j < NoiseGenerator->GridSize.Y; ++j)
		{
			for (int k = 0; k < NoiseGenerator->GridSize.Z; ++k)
			{
				const int Index = i + j * NoiseGenerator->GridSize.X + k * NoiseGenerator->GridSize.X * NoiseGenerator->GridSize.Y;
				const uint8 color{ static_cast<uint8>(NoiseGenerator->McData.Values[Index]) };
				DrawDebugPoint(GetWorld(), NoiseGenerator->McData.Locations[Index], 4, FColor{ color, color, color });
			}
		}
	}
}

void UALG_Base::GenerateMesh()
{
    int index{ 0 };
    NoiseGenerator->ProcMesh->ClearAllMeshSections();
    for (const auto& cube : NoiseGenerator->GridCubes)
    {
        bool isNotFullOrEmpty{ cube.Value.PointValues.Contains(0) && cube.Value.PointValues.Contains(255) };
        if (!isNotFullOrEmpty) {
            continue;
        }

        TArray<FVector> Vertices;
        TArray<int> Triangles;

        // Calculate the 8-bit code representing the cube's configuration
        int CubeConfigIndex = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (cube.Value.PointValues[i] > 0.5f) 
            {
                CubeConfigIndex |= 1 << i;
            }
        }

        for (int i = 0; TriangulationTable[CubeConfigIndex][i] != -1; i += 3)
        {
            const int VertexIndex0 = TriangulationTable[CubeConfigIndex][i];
            const int VertexIndex1 = TriangulationTable[CubeConfigIndex][i + 1];
            const int VertexIndex2 = TriangulationTable[CubeConfigIndex][i + 2];

            const FVector Vertex0 = cube.Value.TrianglePointPositions[VertexIndex0];
            const FVector Vertex1 = cube.Value.TrianglePointPositions[VertexIndex1];
            const FVector Vertex2 = cube.Value.TrianglePointPositions[VertexIndex2];

            Vertices.Add(Vertex0);
            Vertices.Add(Vertex1);
            Vertices.Add(Vertex2);

            Triangles.Add(Vertices.Num() - 3);
            Triangles.Add(Vertices.Num() - 2);
            Triangles.Add(Vertices.Num() - 1);
        }

        NoiseGenerator->ProcMesh->CreateMeshSection(index, Vertices, Triangles, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
        NoiseGenerator->ProcMesh->SetMaterial(index, NoiseGenerator->ProcMeshMaterial);
        index++;
    }
}


// Called every frame
void UALG_Base::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	deltatime = DeltaTime;
}
