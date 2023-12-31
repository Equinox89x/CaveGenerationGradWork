// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "A_Metaball.h"
#include "ALG_Base.h"
#include "ALG_MetaBall.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAVEGENRATION_API UALG_MetaBall : public UALG_Base
{
	GENERATED_BODY()

public:	
	UALG_MetaBall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AA_Metaball> MetaBallToSpawn{ nullptr };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InfluenceRadius{ 11.5 };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InfluenceStrength{ 2.8 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NrOfLayers{ 3 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NrOfBallsPerLayer{ 5 };	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NrOfHeightBallsPerLayer{ 2 };		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsContinous{ false };

	virtual void InitValues(FVector minBoundary, FVector maxBoundary);
	virtual void Update();
	virtual void Draw();
	virtual void Cleanup();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<TSubclassOf<AA_Metaball>> MetaBalls{ nullptr };
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	FVector MinBoundary{};
	FVector MaxBoundary{};
	float DefaultInfluence{ 1000 };	

	TArray<AA_Metaball*> MetaBallObjects{};
	int MaxValue{ 0 }, MinValue{ 0 };
	FCriticalSection CriticalSection;
	int RemapValue(int Value, int OldMin, int OldMax, int NewMin, int NewMax);
	bool IsPointInsideSphere(const FVector& Point, const FVector& SphereCenter, float SphereRadius);

	float SphereSize{ 0 };
	void GenerateMetaBalls();
};