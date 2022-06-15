// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManagerSubsystem.h"

#include "TimeControlled.h"
#include "TimeManagerHelper.h"
#include "Types/TimeManagerProperties.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Types/TMTimerHeapOrder.h"

static const TMap<ETMTimePeriod, int64> PeriodDuration =
{
	{Once, 0},
	{Second, 1},
	{Minute, 60},
	{Hour, 3600},
	{Day, 86400}
};

UTimeManagerSubsystem::UTimeManagerSubsystem()
{
	CurrentDateTime = GetMutableDefault<UTimeManagerProperties>()->InitialDateTime.GetTicks();
	TimeCoefficient = GetMutableDefault<UTimeManagerProperties>()->TimeCoefficient;
}
void UTimeManagerSubsystem::OnGameBegin()
{
}

void UTimeManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasBeenTickedThisFrame())
	{
		return;
	}
	
	/**Ignore time tick, if game is paused*/
	if(CurrentDilation == 0) return;

	const float CurrentTimeCoefficient = TimeCoefficient * CurrentDilation;

	/**Increase time and save previous stamp*/
	CurrentDateTime+= CurrentTimeCoefficient*DeltaTime*ETimespan::TicksPerSecond;

	/**For each timer, except paused - decrease expired time*/
	while (ActiveTimerHeap.Num() > 0)
	{
		FTMTimerHandle TopHandle = ActiveTimerHeap.HeapTop();
		const int32 TopIndex = TopHandle.GetIndex();
		FTMTimerData* Top = &Timers[TopIndex];

		/**Check already expired timers*/
		if (Top->Status == ETMTimerStatus::ActivePendingRemoval)
		{
			ActiveTimerHeap.HeapPop(TopHandle, FTMTimerHeapOrder(Timers), /*bAllowShrinking=*/ false);
			RemoveTimer(TopHandle);
			continue;
		}
		
		if (CurrentDateTime > Top->ExpireTime.GetTicks())
		{
			// Remove it from the heap and store it while we're executing
			ActiveTimerHeap.HeapPop(CurrentlyExecutingTimer, FTMTimerHeapOrder(Timers), /*bAllowShrinking=*/ false);
			Top->Status = ETMTimerStatus::Executing;
			
			checkf(!WillRemoveTimerAssert(CurrentlyExecutingTimer), TEXT("RemoveTimer(CurrentlyExecutingTimer) - due to fail before Execute()"));
			Top->TimerDelegate.Execute();

			// Update Top pointer, in case it has been invalidated by the Execute call
			Top = FindTimer(CurrentlyExecutingTimer);
			checkf(!Top || !WillRemoveTimerAssert(CurrentlyExecutingTimer), TEXT("RemoveTimer(CurrentlyExecutingTimer) - due to fail after Execute()"));
			if (!Top || Top->Status != ETMTimerStatus::Executing)
			{
				break;
			}

			// test to ensure it didn't get cleared during execution
			if (Top)
			{
				// if timer requires a delegate, make sure it's still validly bound (i.e. the delegate's object didn't get deleted or something)
				if (Top->bLoop && (!Top->bRequiresDelegate || Top->TimerDelegate.IsBound()))
				{
					// Put this timer back on the heap
					Top->ExpireTime += Top->Rate;
					Top->Status = ETMTimerStatus::Active;
					ActiveTimerHeap.HeapPush(CurrentlyExecutingTimer, FTMTimerHeapOrder(Timers));
				}
				else
				{
					RemoveTimer(CurrentlyExecutingTimer);
				}

				CurrentlyExecutingTimer.Invalidate();
			}
		}
		else
		{
			// no need to go further down the heap, we can be finished
			break;
		}
	}

	// Timer has been ticked.
	LastTickedFrame = GFrameCounter;

	// If we have any Pending Timers, add them to the Active Queue.
	if( PendingTimerSet.Num() > 0 )
	{
		for (FTMTimerHandle Handle : PendingTimerSet)
		{
			FTMTimerData& TimerToActivate = GetTimer(Handle);

			// Convert from time remaining back to a valid ExpireTime
			TimerToActivate.ExpireTime += CurrentDateTime;
			TimerToActivate.Status = ETMTimerStatus::Active;
			ActiveTimerHeap.HeapPush( Handle, FTMTimerHeapOrder(Timers) );
		}
		PendingTimerSet.Reset();
	}
}
TStatId UTimeManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeManagerSubsystem, STATGROUP_Tickables);
}
void UTimeManagerSubsystem::InitializeStartDateTime(const FDateTime& InitTime)
{
	CurrentDateTime = InitTime.GetTicks();
}

FDateTime UTimeManagerSubsystem::GetCurrentDateTime() const
{
	return CurrentDateTime;
}

FTimespan UTimeManagerSubsystem::RealTimeFloatToGameTimestamp(float RealTime) const
{
	return FTimespan(RealTime * TimeCoefficient * ETimespan::TicksPerSecond);
}

float UTimeManagerSubsystem::GameTimestampToRealTimeFloat(FTimespan GameTime) const
{
	return GameTime.GetTicks() / ETimespan::TicksPerSecond / TimeCoefficient;
}

FTMTimerHandle UTimeManagerSubsystem::InternalBindEventByTime(FTimespan InitialTime, FTMTimerUnifiedDelegate& Event, ETMTimePeriod Period)
{
	FTMTimerHandle Handle;
	if(InitialTime < 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Timing can't be < 0");
		return Handle; 
	}

	
	const FTimespan Delay =  InitialTime.GetTicks() - GetCurrentDateTime().GetTimeOfDay().GetTicks() > 0 ?
		InitialTime.GetTicks() - GetCurrentDateTime().GetTimeOfDay().GetTicks() : 
		FTimespan(ETimespan::TicksPerDay).GetTicks() - (GetCurrentDateTime().GetTimeOfDay().GetTicks()  - InitialTime.GetTicks());
	const FTimespan Rate = PeriodDuration[Period] * ETimespan::TicksPerSecond;

	InternalSetTimer(Handle, Event, Rate, Delay, Period != Once);
	return Handle; 
}
FTMTimerHandle UTimeManagerSubsystem::InternalBindEventByDateAndTime(FDateTime DateTime, FTMTimerUnifiedDelegate& Event)
{
	FTMTimerHandle Handle;
	if(DateTime.GetTicks() < 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Timing can't be < 0");
		return Handle; 
	}

	InternalSetTimer(Handle, Event, FMath::Abs(CurrentDateTime - DateTime.GetTicks()));
	return Handle; 
}

void UTimeManagerSubsystem::ChangeGameTimeDilation(float NewValue)
{
	TArray<AActor*> TimeControlled;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UTimeControlled::StaticClass(), TimeControlled);
	for(AActor* ControlledActor : TimeControlled)
	{
		ITimeControlled::Execute_SetCustomDilation(ControlledActor, NewValue);
	}
	CurrentDilation = NewValue;
}

void UTimeManagerSubsystem::ChangeGameTime(FTimespan NewGameTime)
{
	CurrentDateTime = FDateTime(CurrentDateTime).GetDate().GetTicks() + NewGameTime.GetTicks();
	OnTimeManualChanged.Broadcast();
}

float UTimeManagerSubsystem::GetCurrentGameTimeDilation() const
{
	return CurrentDilation;
}
void UTimeManagerSubsystem::InternalSetTimer(FTMTimerHandle& InOutHandle, FTMTimerUnifiedDelegate& InDelegate, FTimespan InRate, FTimespan InDelay, bool bLoop)
{
	/**Not thread-safety*/
	check(IsInGameThread());
	if (FindTimer(InOutHandle))
	{
		// if the timer is already set, just clear it and we'll re-add it, since 
		// there's no data to maintain.
		ClearTimer(InOutHandle);
	}

	if (InRate.GetTicks() >= 0)
	{
		// set up the new timer
		FTMTimerData NewTimerData;
		NewTimerData.TimerDelegate = MoveTemp(InDelegate);

		NewTimerData.Rate = InRate;
		NewTimerData.bLoop = bLoop;
		NewTimerData.bRequiresDelegate = NewTimerData.TimerDelegate.IsBound();
		
		FTMTimerHandle NewTimerHandle;
		const FTimespan FirstDelay = InDelay >= 0 ? InDelay: InRate;

		/**If already ticked on this frame*/
		if (HasBeenTickedThisFrame())
		{
			NewTimerData.ExpireTime = FirstDelay + CurrentDateTime;
			NewTimerData.Status = ETMTimerStatus::Active;
			NewTimerHandle = AddTimer(MoveTemp(NewTimerData));
			ActiveTimerHeap.HeapPush(NewTimerHandle, FTMTimerHeapOrder(Timers));
		}
		else
		{
			NewTimerData.ExpireTime = FirstDelay;
			NewTimerData.Status = ETMTimerStatus::Pending;
			NewTimerHandle = AddTimer(MoveTemp(NewTimerData));
			PendingTimerSet.Add(NewTimerHandle);
		}

		InOutHandle = NewTimerHandle;
	}
	else
	{
		InOutHandle.Invalidate();
	}
}
void UTimeManagerSubsystem::PauseTimer(FTMTimerHandle InHandle)
{
	// not currently threadsafe
	check(IsInGameThread());

	FTMTimerData* TimerToPause = FindTimer(InHandle);
	if (!TimerToPause || TimerToPause->Status == ETMTimerStatus::Paused)
	{
		return;
	}

	const ETMTimerStatus PreviousStatus = TimerToPause->Status;

	// Remove from previous TArray
	switch(PreviousStatus)
	{
		case ETimerStatus::ActivePendingRemoval:
			{
				break;
			}
		case ETimerStatus::Active:
			{
				const int32 IndexIndex = ActiveTimerHeap.Find(InHandle);
				check(IndexIndex != INDEX_NONE);
				ActiveTimerHeap.HeapRemoveAt(IndexIndex, FTMTimerHeapOrder(Timers), /*bAllowShrinking=*/ false);
				break;
			}
		case ETimerStatus::Pending:
			{
				const int32 NumRemoved = PendingTimerSet.Remove(InHandle);
				check(NumRemoved == 1);
				break;
			}
		case ETimerStatus::Executing:
			{
				check(CurrentlyExecutingTimer == InHandle);

				CurrentlyExecutingTimer.Invalidate();
				break;
			}
		default:
			{
				check(false);
			}
	}

	// Don't pause the timer if it's currently executing and isn't going to loop
	if(PreviousStatus == ETMTimerStatus::Executing && !TimerToPause->bLoop)
	{
		RemoveTimer(InHandle);
	}
	else
	{
		// Add to Paused list
		PausedTimerSet.Add(InHandle);

		// Set new status
		TimerToPause->Status = ETMTimerStatus::Paused;

		// Store time remaining in ExpireTime while paused. Don't do this if the timer is in the pending list.
		if (PreviousStatus != ETMTimerStatus::Pending)
		{
			TimerToPause->ExpireTime -= CurrentDateTime;
		}
	}
}
void UTimeManagerSubsystem::UnPauseTimer(FTMTimerHandle InHandle)
{
	// not currently threadsafe
	check(IsInGameThread());

	FTMTimerData* TimerToUnPause = FindTimer(InHandle);
	if (!TimerToUnPause || TimerToUnPause->Status != ETMTimerStatus::Paused)
	{
		return;
	}

	// Move it out of paused list and into proper TArray
	if(HasBeenTickedThisFrame())
	{
		TimerToUnPause->Status = ETMTimerStatus::Active;
		TimerToUnPause->ExpireTime += CurrentDateTime;
		ActiveTimerHeap.HeapPush(InHandle, FTMTimerHeapOrder(Timers));
	}
	else
	{
		TimerToUnPause->Status = ETMTimerStatus::Pending;
		PendingTimerSet.Add(InHandle);
	}

	// remove from paused list
	PausedTimerSet.Remove(InHandle);
}

FTMTimerData* UTimeManagerSubsystem::FindTimer(const FTMTimerHandle& Handle)
{
	// not currently threadsafe
	check(IsInGameThread());

	if (!Handle.IsValid())
	{
		return nullptr;
	}

	const int32 Index = Handle.GetIndex();
	if (Index < 0 || Index >= Timers.GetMaxIndex() || !Timers.IsAllocated(Index))
	{
		return nullptr;
	}

	FTMTimerData& Timer = Timers[Index];

	if (Timer.Handle != Handle || Timer.Status == ETMTimerStatus::ActivePendingRemoval)
	{
		return nullptr;
	}

	return &Timer;
}
FTMTimerData& UTimeManagerSubsystem::GetTimer(const FTMTimerHandle& Handle)
{
	int32 Index = Handle.GetIndex();
	checkSlow(Index >= 0 && Index < Timers.GetMaxIndex() && Timers.IsAllocated(Index) && Timers[Index].Handle == InHandle);
	FTMTimerData& Timer = Timers[Index];
	return Timer;
}
FTMTimerHandle UTimeManagerSubsystem::AddTimer(FTMTimerData&& TimerData)
{
	const void* TimerIndicesByObjectKey = TimerData.TimerDelegate.GetBoundObject();
	TimerData.TimerIndicesByObjectKey = TimerIndicesByObjectKey;

	int32 NewIndex = Timers.Add(MoveTemp(TimerData));

	FTMTimerHandle Result = GenerateHandle(NewIndex);
	Timers[NewIndex].Handle = Result;

	if (TimerIndicesByObjectKey)
	{
		TSet<FTMTimerHandle>& HandleSet = ObjectToTimers.FindOrAdd(TimerIndicesByObjectKey);

		bool bAlreadyExists = false;
		HandleSet.Add(Result, &bAlreadyExists);
		checkf(!bAlreadyExists, TEXT("A timer with this handle and object has already been added!"));
	}

	return Result;
}
void UTimeManagerSubsystem::RemoveTimer(FTMTimerHandle& Handle)
{
	const FTMTimerData& Data = GetTimer(Handle);

	// Remove TimerIndicesByObject entry if necessary
	if (const void* TimerIndicesByObjectKey = Data.TimerIndicesByObjectKey)
	{
		TSet<FTMTimerHandle>* TimersForObject = ObjectToTimers.Find(TimerIndicesByObjectKey);
		checkf(TimersForObject, TEXT("Removed timer was bound to an object which is not tracked by ObjectToTimers!"));

		const int32 NumRemoved = TimersForObject->Remove(Handle);
		checkf(NumRemoved == 1, TEXT("Removed timer was bound to an object which is not tracked by ObjectToTimers!"));

		if (TimersForObject->Num() == 0)
		{
			ObjectToTimers.Remove(TimerIndicesByObjectKey);
		}
	}

	Timers.RemoveAt(Handle.GetIndex());
	Handle.Invalidate();
}
void UTimeManagerSubsystem::ClearTimer(FTMTimerHandle& Handle)
{
	// not currently threadsafe
	check(IsInGameThread());

	FTMTimerData& Data = GetTimer(Handle);
	switch (Data.Status)
	{
	case ETMTimerStatus::Pending:
		{
			int32 NumRemoved = PendingTimerSet.Remove(Handle);
			check(NumRemoved == 1);
			RemoveTimer(Handle);
			break;
		}
	case ETMTimerStatus::Active:
		{
			Data.Status = ETMTimerStatus::ActivePendingRemoval;
			break;
		}
	case ETMTimerStatus::ActivePendingRemoval:
		{
			// Already removed
			break;
		}

	case ETMTimerStatus::Paused:
		{
			const int32 NumRemoved = PausedTimerSet.Remove(Handle);
			check(NumRemoved == 1);
			RemoveTimer(Handle);
			break;
		}

	case ETMTimerStatus::Executing:
		{
			check(CurrentlyExecutingTimer == Handle);

			// Edge case. We're currently handling this timer when it got cleared.  Clear it to prevent it firing again
			// in case it was scheduled to fire multiple times.
			CurrentlyExecutingTimer.Invalidate();
			RemoveTimer(Handle);
			break;
		}

	default:
		{
			UE_LOG(LogTemp, Error, TEXT("%s"), *UEnum::GetValueAsString(Data.Status));
			//check(false);
			break;
		}
	}
}

bool UTimeManagerSubsystem::IsTimerActive(FTMTimerHandle InHandle)
{
	FTMTimerData const* const TimerData = FindTimer( InHandle );
	return TimerData && TimerData->Status != ETMTimerStatus::Paused;
}
bool UTimeManagerSubsystem::IsTimerPaused(FTMTimerHandle InHandle)
{
	FTMTimerData const* const TimerData = FindTimer( InHandle );
	return TimerData && TimerData->Status != ETMTimerStatus::Active;
}
FTimespan UTimeManagerSubsystem::GetTimerElapsedTime(FTMTimerHandle InHandle)
{
	FTMTimerData const* const TimerData = FindTimer(InHandle);
	if (TimerData)
	{
		switch (TimerData->Status)
		{
		case ETMTimerStatus::Active:
			{
				return TimerData->Rate - (TimerData->ExpireTime - CurrentDateTime);
			}
		case ETMTimerStatus::Executing:
			{
				return TimerData->Rate;
			}
		default:
			// ExpireTime is time remaining for paused timers
			return TimerData->Rate - TimerData->ExpireTime;
		}
	}

	return -1.f;
}
FTimespan UTimeManagerSubsystem::GetTimerRemainingTime(FTMTimerHandle InHandle)
{
	FTMTimerData const* const TimerData = FindTimer(InHandle);
	if (TimerData)
	{
		switch (TimerData->Status)
		{
		case ETMTimerStatus::Active:
			return TimerData->ExpireTime - CurrentDateTime;

		case ETMTimerStatus::Executing:
			return 0.0f;

		default:
			// ExpireTime is time remaining for paused timers
			return TimerData->ExpireTime;
		}
	}

	return -1.f;
}

FTMTimerHandle UTimeManagerSubsystem::GenerateHandle(int32 Index)
{
	uint64 NewSerialNumber = ++LastAssignedSerialNumber;
	if (!ensureMsgf(NewSerialNumber != FTMTimerHandle::MaxSerialNumber, TEXT("Timer serial number has wrapped around!")))
	{
		NewSerialNumber = (uint64)1;
	}

	FTMTimerHandle Result;
	Result.SetIndexAndSerialNumber(Index, NewSerialNumber);
	return Result;
}
bool UTimeManagerSubsystem::WillRemoveTimerAssert(FTMTimerHandle Handle)
{
	const FTMTimerData& Data = GetTimer(Handle);

	// Remove TimerIndicesByObject entry if necessary
	if (const void* TimerIndicesByObjectKey = Data.TimerIndicesByObjectKey)
	{
		const TSet<FTMTimerHandle>* TimersForObject = ObjectToTimers.Find(TimerIndicesByObjectKey);
		if (!TimersForObject)
		{
			return true;
		}

		const FTMTimerHandle* Found = TimersForObject->Find(Handle);
		if (!Found)
		{
			return true;
		}
	}

	return false;
}
