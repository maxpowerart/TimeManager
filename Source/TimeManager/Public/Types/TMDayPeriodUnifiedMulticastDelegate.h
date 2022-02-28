#pragma once

#include "CoreMinimal.h"
#include "TMDayPeriodUnifiedMulticastDelegate.generated.h"

DECLARE_DELEGATE(FTMDayPeriodUpdate);
DECLARE_MULTICAST_DELEGATE(FTMDayPeriodUpdateMulticast);

DECLARE_DYNAMIC_DELEGATE(FTMDynamicDayPeriodUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTMDynamicDayPeriodUpdateMulticast);

USTRUCT()
struct FTMDayPeriodUnifiedMulticastDelegate
{
	GENERATED_BODY()
	
	/** Holds the delegate to call. */
	FTMDayPeriodUpdateMulticast FuncDelegate;
	/** Holds the dynamic delegate to call. */
	FTMDynamicDayPeriodUpdateMulticast FuncDynDelegate;

	FTMDayPeriodUnifiedMulticastDelegate() {};
	FTMDayPeriodUnifiedMulticastDelegate(FTMDayPeriodUpdateMulticast const& D) : FuncDelegate(D) {}
	FTMDayPeriodUnifiedMulticastDelegate(FTMDynamicDayPeriodUpdateMulticast const& D) : FuncDynDelegate(D) {}
	FTMDayPeriodUnifiedMulticastDelegate(FTMDayPeriodUpdateMulticast D) : FuncDelegate(D) {}
	FTMDayPeriodUnifiedMulticastDelegate(FTMDynamicDayPeriodUpdateMulticast D) : FuncDynDelegate(D) {}
	
	void Execute()
	{
		if (FuncDelegate.IsBound())
		{
			FuncDelegate.Broadcast();
		}
		
		if (FuncDynDelegate.IsBound())
		{
			FuncDynDelegate.Broadcast();
		}
	}
	bool IsBound() const
	{
		return ( FuncDelegate.IsBound() || FuncDynDelegate.IsBound());
	}

	void Add(const FTMDayPeriodUpdate& Delegate)
	{
		FuncDelegate.Add(Delegate);
	}
	void Add(const FTMDynamicDayPeriodUpdate& Delegate)
	{
		FuncDynDelegate.Add(Delegate);
	}
	void Remove(const FTMDayPeriodUpdate& Delegate)
	{
		FuncDelegate.Remove(Delegate.GetHandle());
	}
	void Remove(const FTMDynamicDayPeriodUpdate& Delegate)
	{
		FuncDynDelegate.Remove(Delegate);
	}

	void UnbindAll(UObject* Target)
	{
		FuncDelegate.RemoveAll(Target);
		FuncDynDelegate.RemoveAll(Target);
	}

	void Clear()
	{
		FuncDelegate.Clear();
		FuncDynDelegate.Clear();
	}
};