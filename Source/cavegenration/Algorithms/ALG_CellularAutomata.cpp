// Fill out your copyright notice in the Description page of Project Settings.


#include "ALG_CellularAutomata.h"
#include <cavegenration/Generator/NoiseGenerator.h>
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UALG_CellularAutomata::UALG_CellularAutomata()
{
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UALG_CellularAutomata::BeginPlay()
{
	Super::BeginPlay();
}

void UALG_CellularAutomata::InitValues(FVector minBoundary, FVector maxBoundary)
{
    Super::InitValues(minBoundary, maxBoundary);
}

void UALG_CellularAutomata::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UALG_CellularAutomata::Update()
{
    Super::Update();

    //if (timer < 0) {
        for (int i = 0; i < NoiseGenerator->GridSize.X; ++i)
        {
            for (int j = 0; j < NoiseGenerator->GridSize.Y; ++j)
            {
                int Index = i + j * NoiseGenerator->GridSize.X;
                int State = NoiseGenerator->McData.Values[Index];
                int Neighbors = CountNeighbors(i, j);

                if (State == NoiseGenerator->black && Neighbors == 3) {
                    NoiseGenerator->McData.Values[Index] = NoiseGenerator->white;
                }
                else if (State == NoiseGenerator->white && (Neighbors < 2 || Neighbors > 3)) {
                    NoiseGenerator->McData.Values[Index] = NoiseGenerator->black;
                }
            }
        }
    //    timer = DefaultTimer;
    //}
}

int UALG_CellularAutomata::CountNeighbors(int x, int y)
{
    int32 Sum = 0;
    for (int32 i = -1; i < 2; ++i)
    {
        for (int32 j = -1; j < 2; ++j)
        {
            int32 Col = (x + i + static_cast<int>(NoiseGenerator->GridSize.X)) % static_cast<int>(NoiseGenerator->GridSize.X);
            int32 Row = (y + j + static_cast<int>(NoiseGenerator->GridSize.Y)) % static_cast<int>(NoiseGenerator->GridSize.Y);

            int32 NeighborIndex = NoiseGenerator->GridSize.X * Row + Col;
            Sum += (NoiseGenerator->McData.Values[NeighborIndex] == NoiseGenerator->white) ? 1 : 0;
        }
    }
    Sum -= (NoiseGenerator->McData.Values[x + y * NoiseGenerator->GridSize.X] == NoiseGenerator->white) ? 1 : 0;
    return Sum;
}

void UALG_CellularAutomata::Draw()
{
    Super::Draw();
}