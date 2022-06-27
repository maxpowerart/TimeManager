// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManagerHelper.h"

#include "TimeManagerSubsystem.h"

FTMTimerHandle UTimeManagerHelper::K2_SetTimerByGameTimeDuration(const UObject* WorldContextObject, FTMTimerDynamicDelegate Delegate, FTimespan Rate, bool bLoop)
{
	FTMTimerHandle OutHandle;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		World->GetSubsystem<UTimeManagerSubsystem>()->SetTimerByGameTime(OutHandle, Delegate, Rate, -1, bLoop);
		return OutHandle;
	}
	return OutHandle;
}
FTMTimerHandle UTimeManagerHelper::K2_SetTimerByRealTimeDuration(const UObject* WorldContextObject, FTMTimerDynamicDelegate Delegate, FTimespan Rate, bool bLoop)
{
	FTMTimerHandle OutHandle;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		World->GetSubsystem<UTimeManagerSubsystem>()->SetTimerByRealTime(OutHandle, Delegate, Rate, -1, bLoop);
		return OutHandle;
	}
	return OutHandle;
}

void UTimeManagerHelper::K2_ClearAndInvalidateHandler(const UObject* WorldContextObject, FTMTimerHandle& InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			World->GetSubsystem<UTimeManagerSubsystem>()->ClearTimer(InHandle);
		}
	}
}
void UTimeManagerHelper::K2_PauseTimer(const UObject* WorldContextObject, FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			World->GetSubsystem<UTimeManagerSubsystem>()->PauseTimer(InHandle);
		}
	}
}
void UTimeManagerHelper::K2_UnPauseTimer(const UObject* WorldContextObject, FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			World->GetSubsystem<UTimeManagerSubsystem>()->UnPauseTimer(InHandle);
		}
	}
}

void UTimeManagerHelper::K2_SetTimerRate(const UObject* WorldContextObject, FTMTimerHandle InHandle, float NewRate)
{
	if (InHandle.IsValid() && NewRate > 0)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			World->GetSubsystem<UTimeManagerSubsystem>()->SetPlayRate(InHandle, NewRate);
		}
	}
}

FTimespan UTimeManagerHelper::K2_GetTimerElapsedTime(const UObject* WorldContextObject, FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerElapsedTime(InHandle);
		}
	}
	return -1;
}
FTimespan UTimeManagerHelper::K2_GetTimerRemainingTime(const UObject* WorldContextObject, FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerRemainingTime(InHandle);
		}
	}
	return -1;
}

float UTimeManagerHelper::K2_GetTimerElapsedTimeInSeconds(const UObject* WorldContextObject,
	FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerElapsedTimeInSeconds(InHandle);
		}
	}
	return -1;
}

float UTimeManagerHelper::K2_GetTimerRemainingTimeInSeconds(const UObject* WorldContextObject,
	FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerRemainingTimeInSeconds(InHandle);
		}
	}
	return -1;
}

float UTimeManagerHelper::K2_GetTimerExecutionPercent(const UObject* WorldContextObject, FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerElapsedTimeInSeconds(InHandle) /
				(World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerElapsedTimeInSeconds(InHandle) +
					World->GetSubsystem<UTimeManagerSubsystem>()->GetTimerRemainingTimeInSeconds(InHandle));
		}
	}
	return -1;
}

bool UTimeManagerHelper::K2_IsTimerActive(const UObject* WorldContextObject, FTMTimerHandle InHandle)
{
	if (InHandle.IsValid())
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetSubsystem<UTimeManagerSubsystem>()->IsTimerActive(InHandle);
		}
	}
	return false;
}

void UTimeManagerHelper::K2_ChangeGameTimeDilation(const UObject* WorldContextObject, float NewValue)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		World->GetSubsystem<UTimeManagerSubsystem>()->ChangeGameTimeDilation(NewValue);
	}
}

FDateTime UTimeManagerHelper::K2_GetCurrentDateTime(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTimeManagerSubsystem>()->GetCurrentDateTime();
	}
	return 0;
}

float UTimeManagerHelper::K2_GetCurrentGameTimeDilation(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTimeManagerSubsystem>()->GetCurrentGameTimeDilation();
	}
	return 0.f;
}

FTMTimerHandle UTimeManagerHelper::K2_BindEventByTime(const UObject* WorldContextObject, FTimespan InitialTime,
                                                      FTMTimerDynamicDelegate Event, ETMTimePeriod Period)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTimeManagerSubsystem>()->BindEventByTime(InitialTime, Event, Period);
	}
	return FTMTimerHandle();
}
FTMTimerHandle UTimeManagerHelper::K2_BindEventByDateAndTime(const UObject* WorldContextObject, FDateTime DateTime,
	FTMTimerDynamicDelegate Event)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTimeManagerSubsystem>()->BindEventByDateTime(DateTime, Event);
	}
	return FTMTimerHandle();
}

FString UTimeManagerHelper::TimeToString(FTimespan Time)
{
	/**Clamp time to day format*/
	ClampTime(Time);
	
	FString ResultTime;
	if(Time.GetHours() < 10) ResultTime.Append(FString::FromInt(0));
	ResultTime.Append(FString::FromInt(Time.GetHours()));
	ResultTime.AppendChar(':');
	
	if(Time.GetMinutes() < 10) ResultTime.Append(FString::FromInt(0));
	ResultTime.Append(FString::FromInt(Time.GetMinutes()));
	ResultTime.AppendChar(':');
	
	if(Time.GetSeconds() < 10) ResultTime.Append(FString::FromInt(0));
	ResultTime.Append(FString::FromInt(Time.GetSeconds()));
	return ResultTime;
}
bool UTimeManagerHelper::ParseTimeFromString(const FString& TimespanString, FTimespan& OutTimespan)
{
	// get string tokens
	FString TokenString = TimespanString;
	TArray<FString> Tokens;
	TokenString.ParseIntoArray(Tokens, TEXT(":"), true);

	// poor man's token verification
	for (const FString& Token : Tokens)
	{
		if (!Token.IsEmpty() && !Token.IsNumeric())
		{
			return false;
		}
	}

	// add missing tokens
	if (Tokens.Num() < 3)
	{
		Tokens.InsertDefaulted(0, 3 - Tokens.Num());
	}
	else if (Tokens.Num() > 3)
	{
		return false;
	}

	int32 Hours = FCString::Atoi(*Tokens[0]);
	int32 Minutes = FCString::Atoi(*Tokens[1]);
	int32 Seconds = FCString::Atoi(*Tokens[2]);

	if (Hours > 23 || Minutes > 59 || Seconds > 59)
	{
		/**If ends to 0 - remove it and try to repeat*/
		while(Hours > 23 && Hours % 10 == 0) Hours /= 10;
		while(Minutes > 59 && Minutes % 10 == 0) Minutes /= 10;
		while(Seconds > 59 && Seconds % 10 == 0) Seconds /= 10;
		
		if (Hours > 23 || Minutes > 59 || Seconds > 59) return false;
	}

	OutTimespan = FTimespan(0, Hours, Minutes, Seconds);
	return true;
}
FString UTimeManagerHelper::DateToString(FDateTime Date)
{
	FString DateString;
	if(Date.GetDate().GetDay() < 10) DateString.Append(FString::FromInt(0));
	DateString.Append(FString::FromInt(Date.GetDate().GetDay()));
	DateString.Append(".");

	if(Date.GetDate().GetMonth() < 10) DateString.Append(FString::FromInt(0));
	DateString.Append(FString::FromInt(Date.GetDate().GetMonth()));
	DateString.Append(".");

	if(Date.GetDate().GetYear() < 1000)
	{
		const int NecessaryNum = 4 - GetNum(FString::FromInt(Date.GetDate().GetYear()));
		for(int It = 0; It < NecessaryNum; It++)
		{
			DateString.Append(FString::FromInt(0));
		}
	}
	DateString.Append(FString::FromInt(Date.GetDate().GetYear()));
	return DateString;
}
bool UTimeManagerHelper::ParseDateFromString(const FString& DateString, FDateTime& OutDate)
{
	// get string tokens
	FString TokenString = DateString;
	TArray<FString> Tokens;
	TokenString.ParseIntoArray(Tokens, TEXT("."), true);

	// poor man's token verification
	for (const FString& Token : Tokens)
	{
		if (!Token.IsEmpty() && !Token.IsNumeric())
		{
			return false;
		}
	}

	// add missing tokens
	if (Tokens.Num() < 3)
	{
		Tokens.InsertDefaulted(0, 3 - Tokens.Num());
	}
	else if (Tokens.Num() > 3)
	{
		return false;
	}

	int32 Day = FCString::Atoi(*Tokens[0]);
	int32 Month = FCString::Atoi(*Tokens[1]);
	int32 Year = FCString::Atoi(*Tokens[2]);

	if(Month < 1 || Month > 12 || Year < 1 || Year > 9999)
	{
		while(Year > 9999 && Year % 10 == 0) Year /= 10;
		while (Month > 12 && Month % 10 == 0) Month /=10;
		if(Year < 1) Year = 1;
		if(Month < 1) Month = 1;
		if(Month < 1 || Month > 12 || Year < 1 || Year > 9999) return false;
	}
	if(Day < 1 || Day > FDateTime::DaysInMonth(Year, Month))
	{
		while(Day > FDateTime::DaysInMonth(Year, Month) && Day % 10 == 0) Day /= 10;
		if(Day < 1) Day = 1;
		if(Day < 1 || Day > FDateTime::DaysInMonth(Year, Month)) return false;
	}

	OutDate = FDateTime(Year, Month, Day);
	return true;
}

FTimespan UTimeManagerHelper::K2_RealTimeFloatToGameTimestamp(const UObject* WorldContextObject, float RealTime)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTimeManagerSubsystem>()->RealTimeFloatToGameTimestamp(RealTime);
	}
	return 0;
}

float UTimeManagerHelper::K2_GameTimestampToRealTimeFloat(const UObject* WorldContextObject, FTimespan GameTime)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return World->GetSubsystem<UTimeManagerSubsystem>()->GameTimestampToRealTimeFloat(GameTime);
	}
	return 0;
}

FTimespan UTimeManagerHelper::ClampTime(FTimespan& Value)
{
	if(Value < 0)
	{
		Value+= FTimespan(24, 0, 0) * (FMath::Abs(Value.GetDays())+1);
	}
	if(Value.GetDays() > 0)
	{
		Value -= Value.GetDays()*ETimespan::TicksPerDay;
	}
	return Value;
}
int64 UTimeManagerHelper::ToInt64(FTimespan Source)
{
	return Source.GetTicks();
}
