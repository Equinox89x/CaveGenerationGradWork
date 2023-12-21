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
	// Sets default values for this component's properties
	UALG_MetaBall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AA_Metaball>> MetaBalls{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PixelSize = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int sumModifier{ 1000 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int sumModifier2{ 100 };
	float InfluenceRadius{ 0 };

	virtual void InitValues(FVector minBoundary, FVector maxBoundary);
	virtual void Update();
	virtual void Draw();
	virtual void Cleanup();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	TArray<AA_Metaball*> MetaBallObjects{};
	FVector MinBoundary{};
	FVector MaxBoundary{};

	int RemapValue(int Value, int OldMin, int OldMax, int NewMin, int NewMax);
};

int RemapValue(int Value, int OldMin, int OldMax, int NewMin, int NewMax)
{
	return FMath::RoundToInt((Value - OldMin) * (float(NewMax - NewMin) / float(OldMax - OldMin)) + NewMin);
}
