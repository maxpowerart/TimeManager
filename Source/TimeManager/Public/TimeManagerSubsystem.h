// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/TMDayPeriod.h"
#include "Types/TMDayPeriodUnifiedMulticastDelegate.h"
#include "Types/TMTimePeriod.h"
#include "Types/TMTimerData.h"
#include "Types/TMTimerHandle.h"
#include "Types/TMTimerUnifiedDelegate.h"
#include "TimeManagerSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTMOnTimeManualChanged);

UCLASS()
class TIMEMANAGER_API UTimeManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/**Constructor and overrides*/
	UTimeManagerSubsystem();
	void OnGameBegin();
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	/**Initializer methods for overriding start datetime by level (for example)*/
	void InitializeStartDateTime(const FDateTime& InitTime);

	/****************************************/
	/******Time-based events management******/
	/****************************************/

	FORCEINLINE FTMTimerHandle BindEventByTime(FTimespan InitialTime, FTMTimerDelegate const& Event, ETMTimePeriod Period = ETMTimePeriod::Day)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = Event;
		return InternalBindEventByTime(InitialTime, Delegate, Period);
	}
	FORCEINLINE FTMTimerHandle BindEventByTime(FTimespan InitialTime, FTMTimerDynamicDelegate& Event, ETMTimePeriod Period = ETMTimePeriod::Day)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDynDelegate = Event;
		return InternalBindEventByTime(InitialTime, Delegate, Period);
	}
	FORCEINLINE FTMTimerHandle BindEventByTime(FTimespan InitialTime,  TFunction<void(void)>&& Callback, ETMTimePeriod Period = ETMTimePeriod::Day)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncCallback = Callback;
		return InternalBindEventByTime(InitialTime, Delegate, Period);
	}

	FORCEINLINE FTMTimerHandle BindEventByDateTime(FDateTime DateTime, FTMTimerDelegate& Event)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = Event;
		return InternalBindEventByDateAndTime(DateTime, Delegate);
	}
	FORCEINLINE FTMTimerHandle BindEventByDateTime(FDateTime DateTime, FTMTimerDynamicDelegate& Event)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDynDelegate = Event;
		return InternalBindEventByDateAndTime(DateTime, Delegate);
	}
	FORCEINLINE FTMTimerHandle BindEventByDateTime(FDateTime DateTime,  TFunction<void(void)>&& Callback)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncCallback = Callback;
		return InternalBindEventByDateAndTime(DateTime, Delegate);
	}

	FORCEINLINE float GetCurrentTimeCoefficient() const
	{
		return TimeCoefficient;
	}
	FDateTime GetCurrentDateTime() const;

	/**Conversion methods*/
	FTimespan RealTimeFloatToGameTimestamp(float RealTime) const;
	float GameTimestampToRealTimeFloat(FTimespan GameTime) const;

	FORCEINLINE void BindEventByDayPeriod(ETMDayPeriod DayPeriod, FTMDayPeriodUpdate& Event)
	{
		DayPeriodEvents[DayPeriod].Add(Event);
	}
	FORCEINLINE void BindEventByDayPeriod(ETMDayPeriod DayPeriod, FTMDynamicDayPeriodUpdate& Event)
	{
		DayPeriodEvents[DayPeriod].Add(Event);
	}
	FORCEINLINE void UnbindEventByDayPeriod(ETMDayPeriod DayPeriod, FTMDayPeriodUpdate& Event)
	{
		DayPeriodEvents[DayPeriod].Remove(Event);
	}
	FORCEINLINE void UnbindEventByDayPeriod(ETMDayPeriod DayPeriod, FTMDynamicDayPeriodUpdate& Event)
	{
		DayPeriodEvents[DayPeriod].Remove(Event);
	}
	FORCEINLINE void UnbindAllEventByDayPeriod(ETMDayPeriod DayPeriod, UObject* Source)
	{
		DayPeriodEvents[DayPeriod].UnbindAll(Source);
	}
	
	/*****************************************/
	/*******Time-management functional********/
	/*****************************************/
	
	void PauseGame();
	void ResumeGame();
	bool IsGamePaused() const;
	void ChangeGameTimeDilation(float NewValue);
	void ChangeGameTime(FTimespan NewGameTime);
	float GetCurrentGameTimeDilation() const;
	ETMDayPeriod GetCurrentDayPeriod() const;

	UPROPERTY(BlueprintAssignable)
	FTMOnTimeManualChanged OnTimeManualChanged;
	
private:

	FTMTimerHandle InternalBindEventByTime(FTimespan InitialTime, FTMTimerUnifiedDelegate& Event, ETMTimePeriod Period = ETMTimePeriod::Day);
	FTMTimerHandle InternalBindEventByDateAndTime(FDateTime DateTime, FTMTimerUnifiedDelegate& Event);
	void OnDayPeriodUpdated(ETMDayPeriod NewPeriod);
	
	/**Current date/time*/
	double CurrentDateTime;
	/** Track pause state */
	float CurrentDilation = 1.f;
	/** TimeCoefficient = GameTime/RealTime */
	float TimeCoefficient = 1.f;

	/**Time-period based events*/
	UPROPERTY()
	TMap<ETMDayPeriod, FTMDayPeriodUnifiedMulticastDelegate> DayPeriodEvents;
	ETMDayPeriod CurrentDayPeriod;

	/********************************************/
	/************Timer functional****************/
	/********************************************/

public:
	template<class UserClass>
	FORCEINLINE void SetTimerByGameTime(FTMTimerHandle& InOutHandle, UserClass* InObj, typename FTMTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = FTMTimerDelegate::CreateUObject(InObj, InTimerMethod);
		InternalSetTimer(InOutHandle, Delegate, InRate, InDelay, bLoop);
	}
	template<class UserClass>
	FORCEINLINE void SetTimerByGameTime(FTMTimerHandle& InOutHandle, UserClass* InObj, typename FTMTimerDelegate::TUObjectMethodDelegate_Const< UserClass >::FMethodPtr InTimerMethod, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = FTMTimerDelegate::CreateUObject(InObj, InTimerMethod);
		InternalSetTimer(InOutHandle, Delegate, InRate, InDelay, bLoop);
	}

	FORCEINLINE void SetTimerByGameTime(FTMTimerHandle& InOutHandle, FTMTimerDelegate const& InDelegate, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = InDelegate;
		InternalSetTimer(InOutHandle, Delegate, InRate, InDelay, bLoop);
	}
	FORCEINLINE void SetTimerByGameTime(FTMTimerHandle& InOutHandle, FTMTimerDynamicDelegate const& InDelegate, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDynDelegate = InDelegate;
		InternalSetTimer(InOutHandle, Delegate, InRate, InDelay, bLoop);
	}
	FORCEINLINE void SetTimerByGameTime(FTMTimerHandle& InOutHandle, TFunction<void(void)>&& Callback, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncCallback = Callback;
		InternalSetTimer(InOutHandle, Delegate, InRate, InDelay, bLoop);
	}

	template<class UserClass>
	FORCEINLINE void SetTimerByRealTime(FTMTimerHandle& InOutHandle, UserClass* InObj, typename FTMTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = FTMTimerDelegate::CreateUObject(InObj, InTimerMethod);
		InternalSetTimer(InOutHandle, Delegate, InRate  * TimeCoefficient, InDelay, bLoop);
	}
	template<class UserClass>
	FORCEINLINE void SetTimerByRealTime(FTMTimerHandle& InOutHandle, UserClass* InObj, typename FTMTimerDelegate::TUObjectMethodDelegate_Const< UserClass >::FMethodPtr InTimerMethod, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = FTMTimerDelegate::CreateUObject(InObj, InTimerMethod);
		InternalSetTimer(InOutHandle, Delegate, InRate  * TimeCoefficient, InDelay, bLoop);
	}

	FORCEINLINE void SetTimerByRealTime(FTMTimerHandle& InOutHandle, FTMTimerDelegate const& InDelegate, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = InDelegate;
		InternalSetTimer(InOutHandle, Delegate, InRate * TimeCoefficient, InDelay, bLoop);
	}
	FORCEINLINE void SetTimerByRealTime(FTMTimerHandle& InOutHandle, FTMTimerDynamicDelegate const& InDelegate, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDynDelegate = InDelegate;
		InternalSetTimer(InOutHandle, Delegate, InRate * TimeCoefficient, InDelay, bLoop);
	}
	FORCEINLINE void SetTimerByRealTime(FTMTimerHandle& InOutHandle, TFunction<void(void)>&& Callback, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncCallback = Callback;
		InternalSetTimer(InOutHandle, Delegate, InRate * TimeCoefficient, InDelay, bLoop);
	}

	template<class UserClass>
	FORCEINLINE void SetTimerByRealTimeInSeconds(FTMTimerHandle& InOutHandle, UserClass* InObj, typename FTMTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod, float InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = FTMTimerDelegate::CreateUObject(InObj, InTimerMethod);
		InternalSetTimer(InOutHandle, Delegate, InRate  * TimeCoefficient  * ETimespan::TicksPerSecond, InDelay, bLoop);
	}
	template<class UserClass>
	FORCEINLINE void SetTimerByRealTimeInSeconds(FTMTimerHandle& InOutHandle, UserClass* InObj, typename FTMTimerDelegate::TUObjectMethodDelegate_Const< UserClass >::FMethodPtr InTimerMethod, float InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = FTMTimerDelegate::CreateUObject(InObj, InTimerMethod);
		InternalSetTimer(InOutHandle, Delegate, InRate  * TimeCoefficient  * ETimespan::TicksPerSecond, InDelay, bLoop);
	}

	FORCEINLINE void SetTimerByRealTimeInSeconds(FTMTimerHandle& InOutHandle, FTMTimerDelegate const& InDelegate, float InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDelegate = InDelegate;
		InternalSetTimer(InOutHandle, Delegate, InRate * TimeCoefficient * ETimespan::TicksPerSecond, InDelay, bLoop);
	}
	FORCEINLINE void SetTimerByRealTimeInSeconds(FTMTimerHandle& InOutHandle, FTMTimerDynamicDelegate const& InDelegate, float InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncDynDelegate = InDelegate;
		InternalSetTimer(InOutHandle, Delegate, InRate * TimeCoefficient * ETimespan::TicksPerSecond, InDelay, bLoop);
	}
	FORCEINLINE void SetTimerByRealTimeInSeconds(FTMTimerHandle& InOutHandle, TFunction<void(void)>&& Callback, float InRate, FTimespan InDelay = -1, bool bLoop = false)
	{
		FTMTimerUnifiedDelegate Delegate;
		Delegate.FuncCallback = Callback;
		InternalSetTimer(InOutHandle, Delegate, InRate * TimeCoefficient * ETimespan::TicksPerSecond, InDelay, bLoop);
	}
	
	FORCEINLINE float GetTimerElapsedTimeInSeconds(FTMTimerHandle InHandle)
	{
		return static_cast<double>(GetTimerElapsedTime(InHandle).GetTicks()) / ETimespan::TicksPerSecond / TimeCoefficient;
	}
	FORCEINLINE float GetTimerRemainingTimeInSeconds(FTMTimerHandle InHandle)
	{
		return static_cast<double>(GetTimerRemainingTime(InHandle).GetTicks()) / ETimespan::TicksPerSecond / TimeCoefficient;
	}
	
	void PauseTimer(FTMTimerHandle InHandle);
	void UnPauseTimer(FTMTimerHandle InHandle);
	void ClearTimer(FTMTimerHandle& InDelegate);
	bool IsTimerActive(FTMTimerHandle InHandle);
	bool IsTimerPaused(FTMTimerHandle InHandle);
	FTimespan GetTimerElapsedTime(FTMTimerHandle InHandle);
	FTimespan GetTimerRemainingTime(FTMTimerHandle InHandle);
	float GetTimerExecutionPercent(FTMTimerHandle InHandle);
	
private:

	bool FORCEINLINE HasBeenTickedThisFrame() const
	{
		return (LastTickedFrame == GFrameCounter);
	}

	/**Sets timer*/
	void InternalSetTimer(FTMTimerHandle& InOutHandle, FTMTimerUnifiedDelegate& InDelegate, FTimespan InRate, FTimespan InDelay = -1, bool bLoop = false);
	/**Finds timer, if it's possible*/
	FTMTimerData* FindTimer(const FTMTimerHandle& Handle);
	/**Finds timer, throws exception, if it can't be found*/
	FTMTimerData& GetTimer(const FTMTimerHandle& Handle);
	/** Adds a timer from the Timers list, also updating the TimerIndicesByObject map.  Returns the insertion index. */
	FTMTimerHandle AddTimer(FTMTimerData&& TimerData);
	/** Removes a timer from the Timers list at the given index, also cleaning up the TimerIndicesByObject map */
	void RemoveTimer(FTMTimerHandle& Handle);
	/**Generates handle for timer*/
	FTMTimerHandle GenerateHandle(int32 Index);
	/**Checks, should timer be invalidate*/
	bool WillRemoveTimerAssert(FTMTimerHandle Handle);
	
	/** The array of timers - all other arrays will index into this */
	TSparseArray<FTMTimerData> Timers;
	/** Heap of actively running timers. */
	TArray<FTMTimerHandle> ActiveTimerHeap;
	/** Set of paused timers. */
	TSet<FTMTimerHandle> PausedTimerSet;
	/** Set of timers added this frame, to be added after timer has been ticked */
	TSet<FTMTimerHandle> PendingTimerSet;
	/** A map of object pointers to timers with delegates bound to those objects, for quick lookup */
	TMap<const void*, TSet<FTMTimerHandle>> ObjectToTimers;

	/** Index to the timer delegate currently being executed, or INDEX_NONE if none are executing.  Used to handle "timer delegates that manipulate timers" cases. */
	FTMTimerHandle CurrentlyExecutingTimer;
	/** Set this to GFrameCounter when Timer is ticked. To figure out if Timer has been already ticked or not this frame. */
	uint64 LastTickedFrame;
	/** The last serial number we assigned from this timer manager */
	uint64 LastAssignedSerialNumber;
};
