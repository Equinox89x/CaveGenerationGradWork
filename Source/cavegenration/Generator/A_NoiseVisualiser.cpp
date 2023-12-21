// Fill out your copyright notice in the Description page of Project Settings.


#include "A_NoiseVisualiser.h"

// Sets default values
AA_NoiseVisualiser::AA_NoiseVisualiser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh2(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube'"));
	//MeshComponent->SetStaticMesh(mesh2.Object);

    // Create the TextRenderComponent.
    TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
    RootComponent = TextRenderComponent; // Set it as the root component for simplicity.

    // Set properties for the TextRenderComponent.
    TextRenderComponent->SetHorizontalAlignment(EHTA_Center);
    TextRenderComponent->SetWorldSize(50); // Set the size of the text in world units.
    TextRenderComponent->SetWorldScale3D(FVector{ 1 });
}

// Called when the game starts or when spawned
void AA_NoiseVisualiser::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AA_NoiseVisualiser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

