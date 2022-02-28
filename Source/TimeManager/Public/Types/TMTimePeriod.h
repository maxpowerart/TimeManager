#pragma once

#include "CoreMinimal.h"
#include "TMTimePeriod.generated.h"

UENUM(BlueprintType, Blueprintable)
enum ETMTimePeriod 
{
	Once = 0,
	Second = 1,
	Minute = 60,
	Hour = 3600,
	Day = 86400
};