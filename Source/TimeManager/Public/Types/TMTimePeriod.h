#pragma once

#include "CoreMinimal.h"
#include "TMTimePeriod.generated.h"

UENUM(BlueprintType, Blueprintable)
enum ETMTimePeriod 
{
	Once,
	Second,
	Minute,
	Hour,
	Day 
};