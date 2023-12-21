// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALG_Base.h"
#include "ALG_CellularAutomata.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAVEGENRATION_API UALG_CellularAutomata : public UALG_Base
{
	GENERATED_BODY()

public:	
	UALG_CellularAutomata();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<int> GridData;

	int CountNeighbors(int x, int y);

	virtual void InitValues(FVector minBoundary, FVector maxBoundary);
	virtual void Update();
	virtual void Draw();

protected:
	virtual void BeginPlay() override;

private:	
};
