// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_Metaball.generated.h"

UCLASS()
class CAVEGENRATION_API AA_Metaball : public AActor
{
	GENERATED_BODY()
	
public:	
	AA_Metaball();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MetaballMesh{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed{ 300 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHeightBall{ false };
	int LayerNr{ 0 };

	void InitValues(FVector minBoundary, FVector maxBoundary, FVector spawnLocation);

	virtual void Tick(float DeltaTime) override;

	const float CalculateInfluence(FVector cubeCornerPos, float influenceStrength) const;

protected:
	virtual void BeginPlay() override;

private:	
	FVector MinBoundary{};
	FVector MaxBoundary{};

	FVector CurrentVelocity{ };

	void SetArea(FVector minBoundary, FVector maxBoundary);
};
