#pragma once

#include "CoreMinimal.h"
#include "TMTimerStatus.generated.h"

UENUM()
enum class ETMTimerStatus : uint8
{
	Pending,
	Active,
	Paused,
	Executing,
	ActivePendingRemoval
};