#pragma once

#include "CoreMinimal.h"
#include "TMTimerHandle.h"
#include "TMTimerStatus.h"
#include "TMTimerUnifiedDelegate.h"
#include "TMTimerData.generated.h"

USTRUCT()
struct FTMTimerData
{
	GENERATED_BODY()

	/** If true, this timer will loop indefinitely.  Otherwise, it will be destroyed when it expires. */
	bool bLoop : 1;

	/** If true, this timer was created with a delegate to call (which means if the delegate becomes invalid, we should invalidate the timer too). */
	uint8 bRequiresDelegate : 1;

	/** Timer Status */
	ETMTimerStatus Status;
	
	/** Time between set and fire, or repeat frequency if looping. */
	FTimespan Rate;

	/** Game timing until execution of current iteration of this timer */
	FTimespan ExpireTime;

	/** Current play rate of this timer*/
	float PlayRate = 1.f;

	/** Holds the delegate to call. */
	FTMTimerUnifiedDelegate TimerDelegate;

	/** Handle representing this timer */
	FTMTimerHandle Handle;

	/** This is the key to the TimerIndicesByObject map - this is kept so that we can look up even if the referenced object is expired */
	const void* TimerIndicesByObjectKey;

	FTMTimerData()
		: bLoop(false)
		, bRequiresDelegate(false)
		, Status(ETMTimerStatus::Active)
		, Rate(0)
		, ExpireTime(0)
	{}

	// Movable only
	/*FTMTimerData(FTMTimerData&&) = default;
	FTMTimerData(const FTMTimerData&) = delete;
	FTMTimerData& operator=(FTMTimerData&&) = default;
	FTMTimerData& operator=(const FTMTimerData&) = delete;*/
};