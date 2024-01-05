// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Metaball.h"

// Sets default values
AA_Metaball::AA_Metaball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MetaballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MetaballMesh"));

    CurrentVelocity = FMath::VRand();
    if (!IsHeightBall) CurrentVelocity.Z = 0;
}

// Called when the game starts or when spawned
void AA_Metaball::BeginPlay()
{
	Super::BeginPlay();
	
}

void AA_Metaball::SetArea(FVector minBoundary, FVector maxBoundary)
{
    MinBoundary = minBoundary;
    MaxBoundary = maxBoundary;
}

void AA_Metaball::InitValues(FVector minBoundary, FVector maxBoundary, FVector spawnLocation)
{
    SetActorLocation(spawnLocation);
    SetArea(minBoundary, maxBoundary);
    SetActorScale3D(FVector(0.2));
}

void AA_Metaball::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector NewLocation{ GetActorLocation() + (CurrentVelocity * (MovementSpeed * DeltaTime)) };

    if (NewLocation.X < MinBoundary.X || NewLocation.X > MaxBoundary.X)
    {
        CurrentVelocity.X *= -1;
        NewLocation.X += 50 * CurrentVelocity.X;
    }

    if (NewLocation.Y < MinBoundary.Y || NewLocation.Y > MaxBoundary.Y)
    {
        CurrentVelocity.Y *= -1;
        NewLocation.Y += 50 * CurrentVelocity.Y;
    }

    if (NewLocation.Z < MinBoundary.Z || NewLocation.Z > MaxBoundary.Z)
    {
        CurrentVelocity.Z *= -1;
        NewLocation.Z += 50 * CurrentVelocity.Z;
    }

    SetActorLocation(NewLocation);
}

const float AA_Metaball::CalculateInfluence(FVector cubeCornerPos, float influenceStrength) const
{
    const float distanceSquared{ static_cast<float>(FVector::DistSquared(cubeCornerPos, GetActorLocation())) };
    return influenceStrength / FMath::Max(1.0f, distanceSquared);
}

