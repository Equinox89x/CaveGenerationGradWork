// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include <Kismet/GameplayStatics.h>

AMeshGenerator::AMeshGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = ProcMesh;
	ProcMesh->bUseAsyncCooking = true;
}

void AMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
	NoiseGenerator = Cast<ANoiseGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ANoiseGenerator::StaticClass()));
	BiomeGenerator = Cast<ABiomeGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ABiomeGenerator::StaticClass()));
}

void AMeshGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeshGenerator::GenerateMesh()
{
	NoiseGenerator->HandlePreMeshGen();

	int index{ 0 };
	ProcMesh->ClearAllMeshSections();
	BiomeGenerator->Reset();

	//Marching Cubes Algorithm
	for (const auto& cube : NoiseGenerator->GridCubesForMesh)
	{
		TArray<FVector> Vertices;
		TArray<int> Triangles;

		// Calculate the 8-bit code representing the cube's configuration
		int CubeConfigIndex{ 0 };
		for (int i = 0; i < 8; ++i)
		{
			if (cube.Value.PointValues[i] > 0.5f)
			{
				CubeConfigIndex |= 1 << i;
			}
		}

		//detect what biome to spawn
		const auto& selectedBiome{ BiomeGenerator->GetSelectedBiome(cube.Value.BiomeValues) };

		for (int i = 0; TriangulationTable[CubeConfigIndex][i] != -1; i += 3)
		{
			const int VertexIndex0{ TriangulationTable[CubeConfigIndex][i] };
			const int VertexIndex1{ TriangulationTable[CubeConfigIndex][i + 1] };
			const int VertexIndex2{ TriangulationTable[CubeConfigIndex][i + 2] };

			const FVector Vertex0{ cube.Value.TrianglePointPositions[VertexIndex0].Position };
			const FVector Vertex1{ cube.Value.TrianglePointPositions[VertexIndex1].Position };
			const FVector Vertex2{ cube.Value.TrianglePointPositions[VertexIndex2].Position };

			Vertices.Add(Vertex0);
			Vertices.Add(Vertex1);
			Vertices.Add(Vertex2);

			Triangles.Add(Vertices.Num() - 3);
			Triangles.Add(Vertices.Num() - 2);
			Triangles.Add(Vertices.Num() - 1);

			BiomeGenerator->CreateFloorAndCeiling(Vertex0, Vertex1, Vertex2, selectedBiome.Color);
		}

		ProcMesh->CreateMeshSection(index, Vertices, Triangles, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
		ProcMesh->SetMaterial(index, selectedBiome.Wall);

		index++;
	}

	UStaticMesh* GeneratedStaticMesh = GenerateStaticMesh();
	StaticMeshComponent->SetStaticMesh(GeneratedStaticMesh);
	ProcMesh->ClearAllMeshSections();
}

UStaticMesh* AMeshGenerator::GenerateStaticMesh()
{
	FMeshDescription MeshDescription{ BuildMeshDescription(ProcMesh) };

	if (MeshDescription.Polygons().Num() > 0)
	{
		// Create StaticMesh object
		UStaticMesh* StaticMesh{ NewObject<UStaticMesh>(ProcMesh) };
		StaticMesh->InitResources();

		StaticMesh->SetLightingGuid();

		// Add source to new StaticMesh
		auto Desc = StaticMesh->CreateStaticMeshDescription();
		Desc->SetMeshDescription(MeshDescription);
		// buildSimpleCol = false, cause it creates box collision based on mesh bounds
		StaticMesh->BuildFromStaticMeshDescriptions({ Desc }, false);

		StaticMesh->CreateBodySetup();
		UBodySetup* NewBodySetup{ StaticMesh->GetBodySetup() };
		NewBodySetup->BodySetupGuid = FGuid::NewGuid();
		NewBodySetup->AggGeom.ConvexElems = ProcMesh->ProcMeshBodySetup->AggGeom.ConvexElems;
		NewBodySetup->bGenerateMirroredCollision = true;
		NewBodySetup->bDoubleSidedGeometry = true;
		NewBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		NewBodySetup->CreatePhysicsMeshes();

		// materials
		TSet<UMaterialInterface*> UniqueMaterials;
		for (int32 i = 0; i < ProcMesh->GetNumSections(); i++)
		{
			FProcMeshSection* ProcSection = ProcMesh->GetProcMeshSection(i);
			UMaterialInterface* Material = ProcMesh->GetMaterial(i);
			UniqueMaterials.Add(Material);
		}

		// Copy materials to new mesh
		for (auto* Material : UniqueMaterials)
		{
			StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material));
		}


		// Uncallable in game runtime
		// StaticMesh->Build(false);

		return StaticMesh;
	}

	return nullptr;
}
