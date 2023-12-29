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