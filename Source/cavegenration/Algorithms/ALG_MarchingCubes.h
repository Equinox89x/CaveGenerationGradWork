// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALG_Base.h"
#include "ALG_MarchingCubes.generated.h"

UCLASS()
class CAVEGENRATION_API UALG_MarchingCubes : public UALG_Base
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UALG_MarchingCubes();

	virtual void Update();
	virtual void Draw();
	virtual void Cleanup() {};

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

};
