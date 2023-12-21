// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TextRenderComponent.h>
#include "A_NoiseVisualiser.generated.h"

UCLASS()
class CAVEGENRATION_API AA_NoiseVisualiser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AA_NoiseVisualiser();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite);
	//UStaticMesh* mesh{ nullptr };

	//UPROPERTY(EditAnywhere, BlueprintReadWrite);
	//UStaticMeshComponent* MeshComponent{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UTextRenderComponent* TextRenderComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
