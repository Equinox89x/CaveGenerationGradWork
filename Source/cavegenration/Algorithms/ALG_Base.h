// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALG_Base.generated.h"

class ANoiseGenerator;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAVEGENRATION_API UALG_Base : public UActorComponent
{
	GENERATED_BODY()

public:	

	UALG_Base();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultTimer{ 1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasDirectNullValue{ false };

	virtual void InitValues(FVector MinBoundary, FVector Maxboundary) {};
	virtual void Cleanup() {};
	virtual void Update();
	virtual void Draw();
	virtual void HandlePreMeshGen();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	ANoiseGenerator* NoiseGenerator{ nullptr };
protected:

	virtual void BeginPlay() override;

	TArray<TArray<int>> Cubes{  };

	float timer{ 1 };
	float deltatime{ 0 };

	float MinValue{ -1 };
	float MaxValue{ 1 };

	//int GetIndex(int x, int y, int z);

private:	


};
