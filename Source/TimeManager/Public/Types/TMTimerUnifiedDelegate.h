#pragma once

#include "CoreMinimal.h"
#include "TMTimerUnifiedDelegate.generated.h"

DECLARE_DELEGATE(FTMTimerDelegate);
DECLARE_DYNAMIC_DELEGATE(FTMTimerDynamicDelegate);

USTRUCT()
struct FTMTimerUnifiedDelegate
{
	GENERATED_BODY()
	
	/** Holds the delegate to call. */
	FTMTimerDelegate FuncDelegate;
	/** Holds the dynamic delegate to call. */
	FTMTimerDynamicDelegate FuncDynDelegate;
	/** Holds the TFunction callback to call. */
	TFunction<void(void)> FuncCallback;

	FTMTimerUnifiedDelegate() {};
	FTMTimerUnifiedDelegate(FTMTimerDelegate const& D) : FuncDelegate(D) {}
	FTMTimerUnifiedDelegate(FTMTimerDynamicDelegate const& D) : FuncDynDelegate(D) {}
	FTMTimerUnifiedDelegate(TFunction<void(void)>&& Callback) : FuncCallback(MoveTemp(Callback)) {}
	FTMTimerUnifiedDelegate(FTMTimerDelegate D) : FuncDelegate(D) {}
	FTMTimerUnifiedDelegate(FTMTimerDynamicDelegate D) : FuncDynDelegate(D) {}
	
	void Execute()
	{
		if (FuncDelegate.IsBound())
		{
#if STATS
			TStatId StatId = TStatId();
			UObject* Object = FuncDelegate.GetUObject();
			if (Object)
			{
				StatId = Object->GetStatID();
			}
			FScopeCycleCounter Context(StatId);
#endif
			FuncDelegate.Execute();
		}
		else if (FuncDynDelegate.IsBound())
		{
			FuncDynDelegate.ProcessDelegate<UObject>(nullptr);
		}
		else if ( FuncCallback )
		{
			FuncCallback();
		}
	}
	bool IsBound() const
	{
		return ( FuncDelegate.IsBound() || FuncDynDelegate.IsBound() || FuncCallback );
	}

	const void* GetBoundObject() const
	{
		if (FuncDelegate.IsBound())
		{
			return FuncDelegate.GetObjectForTimerManager();
		}
		else if (FuncDynDelegate.IsBound())
		{
			return FuncDynDelegate.GetUObject();
		}

		return nullptr;
	}

	void Unbind()
	{
		FuncDelegate.Unbind();
		FuncDynDelegate.Unbind();
		FuncCallback = nullptr;
	}

	/** Utility to output info about delegate as a string. */
	FString ToString() const;
};