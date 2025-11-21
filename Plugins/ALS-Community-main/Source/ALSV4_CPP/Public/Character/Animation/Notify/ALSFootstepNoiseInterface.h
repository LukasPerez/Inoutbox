#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ALSFootstepNoiseInterface.generated.h"

UINTERFACE(Blueprintable)
class ALSV4_CPP_API UALSFootstepNoiseInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Objetos que quieren reaccionar a los footsteps de ALS
 */
class ALSV4_CPP_API IALSFootstepNoiseInterface
{
	GENERATED_BODY()

public:

	/** Notificación de footstep de ALS */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="ALS|Footstep")
	void HandleALSFootstepNoise(const FVector& Location, float Loudness);
};
