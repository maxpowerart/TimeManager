#pragma once

#include "CoreMinimal.h"
#include "TMTimerData.h"

struct FTMTimerHeapOrder
{
	explicit FTMTimerHeapOrder(const TSparseArray<FTMTimerData>& InTimers)
		: Timers(InTimers)
		, NumTimers(InTimers.Num())
	{
	}

	bool operator()(FTMTimerHandle LhsHandle, FTMTimerHandle RhsHandle) const
	{
		const int32 LhsIndex = LhsHandle.GetIndex();
		const int32 RhsIndex = RhsHandle.GetIndex();

		const FTMTimerData& LhsData = Timers[LhsIndex];
		const FTMTimerData& RhsData = Timers[RhsIndex];

		return LhsData.ExpireTime < RhsData.ExpireTime;
	}

	const TSparseArray<FTMTimerData>& Timers;
	int32 NumTimers;
};