// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EAlgorithms {
	CELLULAR_AUTOMATA = 0,
	META_BALLS = 1,
	MARCHING_CUBES = 2,
	VORONOI = 3,
	RANDOM_WALK = 4,
	LSYSTEMS = 5,
};

UENUM()
enum class ObjectType {
	Floor,
	Ceiling
};

//USTRUCT()
//struct FMeshData {
//	TArray<TArray<FVector>> Squares;
//	TArray<TArray<int>> indexes;
//	
//};

//float RemapValue(float Value, int OldMin, int OldMax, int NewMin, int NewMax);

/*Good caves :
	1. Default Marching cubes
	2. Marching Cubes:
		NoiseGenerator:
			min & max air: 126, 255
			perlin scale: 0.001
			1. seed: 293.143894, 5330.922236, 22317293.131445
			2. seed: 293.143894,329.159989,34.637587
			grid size: 30, 30, 10
			cube size: 115
		BiomeGenerator:
			seed: 1337
			scale: 0.2
			multiplier: 1
			treshold: 127
*/
