// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ENoiseMaps {
	PERLIN,
	SIMPLEX,

};

UENUM()
enum class EAlgorithms {
	CELLULAR_AUTOMATA = 0,
	META_BALLS = 1,
	MARCHING_CUBES = 2,
	VORONOI = 3,
	RANDOM_WALK = 4,
	LSYSTEMS = 5,
};

//USTRUCT()
//struct FMeshData {
//	TArray<TArray<FVector>> Squares;
//	TArray<TArray<int>> indexes;
//	
//};