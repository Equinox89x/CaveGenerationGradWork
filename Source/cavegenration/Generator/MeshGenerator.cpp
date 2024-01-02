// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include <Kismet/GameplayStatics.h>
#include "PackageTools.h"
#include "Misc/PackageName.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include <AssetToolsModule.h>
#include "Interfaces/ITargetPlatform.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "UObject/SavePackage.h"

AMeshGenerator::AMeshGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	OuterWallComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OuterWallComponent"));
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

	/*TArray<TMap<FVector, FMCCube>*> GridCubesArray {
	   &NoiseGenerator->GridCubesForMesh,
	   &NoiseGenerator->GridCubesForWall
	};

	for (const TMap<FVector, FMCCube>* gridCubesPair : GridCubesArray)
	{
		for (const auto& cube : *gridCubesPair)
		{
			TArray<FVector> Vertices;
			TArray<int> Triangles;

			int CubeConfigIndex{ 0 };
			for (int i = 0; i < 8; ++i)
			{
				if (cube.Value.PointValues[i] > 0.5f)
				{
					CubeConfigIndex |= 1 << i;
				}
			}

			const FBiome& selectedBiome{ BiomeGenerator->GetSelectedBiome(cube.Value, IsFixedValueBiome) };

			for (int i = 0; TriangulationTable[CubeConfigIndex][i] != -1; i += 3)
			{
				const int VertexIndex0{ TriangulationTable[CubeConfigIndex][i] };
				const int VertexIndex1{ TriangulationTable[CubeConfigIndex][i + 1] };
				const int VertexIndex2{ TriangulationTable[CubeConfigIndex][i + 2] };

				const FVector Vertex0{ cube.Value.TrianglePointPositions[VertexIndex0].Position };
				const FVector Vertex1{ cube.Value.TrianglePointPositions[VertexIndex1].Position };
				const FVector Vertex2{ cube.Value.TrianglePointPositions[VertexIndex2].Position };

				Vertices.Add(Vertex2);
				Vertices.Add(Vertex1);
				Vertices.Add(Vertex0);

				Triangles.Add(Vertices.Num() - 3);
				Triangles.Add(Vertices.Num() - 2);
				Triangles.Add(Vertices.Num() - 1);

				BiomeGenerator->CreateFloorAndCeiling(Vertex0, Vertex1, Vertex2, selectedBiome.Color);
			}

			ProcMesh->CreateMeshSection(index, Vertices, Triangles, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
			ProcMesh->SetMaterial(index, selectedBiome.Wall);
			index++;
		}
	}*/

	// Marching Cubes Algorithm
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

		// Detect what biome to spawn
		const FBiome& selectedBiome{ BiomeGenerator->GetSelectedBiome(cube.Value, IsFixedValueBiome) };

		for (int i = 0; TriangulationTable[CubeConfigIndex][i] != -1; i += 3)
		{
			const int VertexIndex0{ TriangulationTable[CubeConfigIndex][i] };
			const int VertexIndex1{ TriangulationTable[CubeConfigIndex][i + 1] };
			const int VertexIndex2{ TriangulationTable[CubeConfigIndex][i + 2] };

			const FVector Vertex0{ cube.Value.TrianglePointPositions[VertexIndex0].Position };
			const FVector Vertex1{ cube.Value.TrianglePointPositions[VertexIndex1].Position };
			const FVector Vertex2{ cube.Value.TrianglePointPositions[VertexIndex2].Position };

			// To flip faces, switch the vertex order (Vertex0, 1 ,2)
			Vertices.Add(Vertex2);
			Vertices.Add(Vertex1);
			Vertices.Add(Vertex0);

			Triangles.Add(Vertices.Num() - 3);
			Triangles.Add(Vertices.Num() - 2);
			Triangles.Add(Vertices.Num() - 1);

			BiomeGenerator->CreateFloorAndCeiling(Vertex0, Vertex1, Vertex2, selectedBiome.Color);
		}

		ProcMesh->CreateMeshSection(index, Vertices, Triangles, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
		ProcMesh->SetMaterial(index, selectedBiome.Wall);

		index++;
	}	

	//Generate walls/floors
	ProcMesh->CreateMeshSection(index-1, NoiseGenerator->GridCubesForWall->Vertices, NoiseGenerator->GridCubesForWall->Triangles, TArray<FVector>{}, TArray<FVector2D>{}, TArray<FColor>{}, TArray<FProcMeshTangent>{}, true);
	//ProcMesh->SetMaterial(index, selectedBiome.Wall);

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

		// Materials
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

		//// Save the mesh
		//FString AssetName = TEXT("GeneratedMesh");
		//FString PackagePath = FString::Printf(TEXT("/Game/GeneratedMeshes"));
		//FString MeshPackageName = FPackageName::ObjectPathToPackageName(PackagePath / AssetName);

		//UPackage* Package = CreatePackage(nullptr, *MeshPackageName);
		//Package->FullyLoad();

		//FString MeshAssetPath = MeshPackageName;
		//Package->MarkPackageDirty();

		//// Save the package
		//FString ErrorMessage;
		////ITargetPlatformManagerModule& TPMModule = GetTargetPlatformManagerRef();
		////const TArray<ITargetPlatform*>& AvailablePlatforms = TPMModule.GetTargetPlatforms();

		//FSavePackageArgs args{};
		//args.SaveFlags = SAVE_NoError;
		//args.Error = GError;
		//args.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
		//args.bWarnOfLongFilename = true;
		//args.bSlowTask = false;
		//args.FinalTimeStamp = FDateTime::Now();
		//
		//bool bSaved = UPackage::SavePackage(Package, StaticMesh, *MeshAssetPath, args);
		////bSaved = UPackage::SavePackage(Package, StaticMesh, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *MeshAssetPath, GError, nullptr, false, true, SAVE_NoError, AvailablePlatforms[23], FDateTime::Now(), false);


		//if (bSaved)
		//{
		//	FAssetRegistryModule::AssetCreated(StaticMesh);
		//	UE_LOG(LogTemp, Warning, TEXT("Saved mesh to %s"), *MeshAssetPath);
		//}
		//else
		//{
		//	GError->Log(ErrorMessage);
		//	UE_LOG(LogTemp, Error, TEXT("Failed to save mesh: %s"), *ErrorMessage);
		//}

		return StaticMesh;
	}

	return nullptr;
}
