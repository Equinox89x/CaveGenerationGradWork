// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <cavegenration/noise/FastNoise.h>
#include "../GameEnums.h"
#include "BiomeGenerator.generated.h"
class NoiseGenerator;
struct FMCCube;

USTRUCT(BlueprintType)
struct FBiome {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* Wall{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Color{ 0 };
};

UCLASS()
class CAVEGENRATION_API ABiomeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABiomeGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BiomeNoise)
	float Seed{ 123 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BiomeNoise)
	float NoiseScale{ 0.1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BiomeNoise)
	float NoiseMultiplier{ 0.1 };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BiomeNoise)
	int Treshold{ 30 };		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BiomeNoise)
	TArray<FBiome> BiomeMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CeilingAndFloor)
	float FloorMaxAngle{ 0.1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CeilingAndFloor)
	float CeilingMaxAngle{ 0.9 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CeilingAndFloor)
	UInstancedStaticMeshComponent* InstancedFloorMesh;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CeilingAndFloor)
	UInstancedStaticMeshComponent* InstancedCeilingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BiomeGeneation)
	int NrOfBiomeLayers{3};	


	UFUNCTION(BlueprintCallable)
	void GenerateBiome();		
	UFUNCTION(BlueprintCallable)
	void ClearBiome();	
	UFUNCTION(BlueprintCallable)
	void CycleBiomeNoise();

	virtual void Tick(float DeltaTime) override;

	void CreateFloorAndCeiling(FVector Vertex0, FVector Vertex1, FVector Vertex2, FVector color);
	void Reset();
	void FillColorArr();
	float FindClosestFloat(float TargetNumber, bool isDirectAccess = false);
	FBiome& GetSelectedBiome(const FMCCube& cube, bool isFixedValueBiome = false);
	void InitNoise();
	int GetBiomeNoise(FVector position);
	void InitBiomeLayers(FVector minBoundary, FVector maxBoundary);
	FastNoiseLite::NoiseType SelectedNoise{ FastNoiseLite::NoiseType_OpenSimplex2 };

protected:
	virtual void BeginPlay() override;

private:	
	FastNoiseLite FastNoise{};

	int NrOfColors{ 4 };	
	TMap<float, FBiome> NoiseBiomes;

	TMap<int, FVector2D> LayerBiomes{};

	TMap<FVector, FVector> CeilingLocations;
	TMap<FVector, FVector> FloorLocations;

	bool IsTriangleLookingUp(const FVector& Vertex1, const FVector& Vertex2, const FVector& Vertex3);
	bool IsTriangleLookingDown(const FVector& Vertex1, const FVector& Vertex2, const FVector& Vertex3);
};
