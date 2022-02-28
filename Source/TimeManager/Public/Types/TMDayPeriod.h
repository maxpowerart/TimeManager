#pragma once

#include "CoreMinimal.h"
#include "TMDayPeriod.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class ETMDayPeriod : uint8
{
	Dusk,
	Dawn
};