#pragma once

#include "CoreMinimal.h"
#include "TimeManagerProperties.generated.h"

UCLASS(Config=Game, defaultconfig)
class TIMEMANAGER_API UTimeManagerProperties : public UObject
{
	GENERATED_BODY()
	
public:
	UTimeManagerProperties();

	/** Coefficient between real and game time
	 *  TimeCoefficient = GameTime/RealTime
	 */
	UPROPERTY(EditAnywhere, Config, Meta = (ClampMin = 1.f, UIMin = 1.f))
	float TimeCoefficient;

	UPROPERTY(EditAnywhere, Config)
	FDateTime InitialDateTime;
};