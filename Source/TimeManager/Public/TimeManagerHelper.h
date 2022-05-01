// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/TMTimePeriod.h"
#include "Types/TMTimerHandle.h"
#include "Types/TMTimerUnifiedDelegate.h"
#include "TimeManagerHelper.generated.h"

UCLASS()
class TIMEMANAGER_API UTimeManagerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Set Timer by Game Time Duration", WorldContext="WorldContextObject", ScriptName = "SetTimerByGameTimeDuration"), Category="TM|Timer")
	static FTMTimerHandle K2_SetTimerByGameTimeDuration(const UObject* WorldContextObject, FTMTimerDynamicDelegate Delegate, FTimespan Rate, bool bLoop = false);
	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Set Timer by Real Time Duration", WorldContext="WorldContextObject", ScriptName = "SetTimerByRealTimeDuration"), Category="TM|Timer")
	static FTMTimerHandle K2_SetTimerByRealTimeDuration(const UObject* WorldContextObject, FTMTimerDynamicDelegate Delegate, FTimespan Rate, bool bLoop = false);
	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Clear and Invalidate Timer Handler", WorldContext="WorldContextObject", ScriptName = "ClearAndInvalidateTimerHandler"), Category="TM|Timer")
	static void K2_ClearAndInvalidateHandler(const UObject* WorldContextObject, UPARAM(ref) FTMTimerHandle& InHandle);
	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Pause Timer", WorldContext="WorldContextObject", ScriptName = "PauseTimer"), Category="TM|Timer")
	static void K2_PauseTimer(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintCallable,  Meta=(DisplayName = "Unpause Timer", WorldContext="WorldContextObject", ScriptName = "UnpauseTimer"), Category="TM|Timer")
	static void K2_UnPauseTimer(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Timer Elapsed Time",WorldContext="WorldContextObject", ScriptName = "GetTimerelapsedTime"), Category="TM|Timer")
	static FTimespan K2_GetTimerElapsedTime(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Timer Remaining Time",WorldContext="WorldContextObject", ScriptName = "GetTimerRemainingTime"), Category="TM|Timer")
	static FTimespan K2_GetTimerRemainingTime(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Timer Elapsed Time In Seconds",WorldContext="WorldContextObject", ScriptName = "GetTimerElapsedTimeInSeconds"), Category="TM|Timer")
	static float K2_GetTimerElapsedTimeInSeconds(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Timer Remaining Time In Seconds",WorldContext="WorldContextObject", ScriptName = "GetTimerRemainingTimeInSeconds"), Category="TM|Timer")
	static float K2_GetTimerRemainingTimeInSeconds(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Timer Execution Percent",WorldContext="WorldContextObject", ScriptName = "GetTimerExecutionPercent"), Category="TM|Timer")
	static float K2_GetTimerExecutionPercent(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Is Timer Active",WorldContext="WorldContextObject", ScriptName = "IsTimerActive"), Category="TM|Timer")
	static bool K2_IsTimerActive(const UObject* WorldContextObject, FTMTimerHandle InHandle);
	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Change Game Time Dilation", WorldContext="WorldContextObject", ScriptName = "ChangeGameTimeDilation", UIMin = 0.000020, ClampMax = 20.f, UIMax = 20.f), Category = "TM|State")
	static void K2_ChangeGameTimeDilation(const UObject* WorldContextObject, float NewValue);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Current Game Time", WorldContext="WorldContextObject", ScriptName = "GetCurrentGameTime"), Category = "TM|State")
	static FDateTime K2_GetCurrentDateTime(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Get Current Game Time Dilation",WorldContext="WorldContextObject", ScriptName = "GetCurrentGameTimeDilation"), Category = "TM|State")
	static float K2_GetCurrentGameTimeDilation(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Meta=(DisplayName = "Bind Event by Timing", WorldContext="WorldContextObject", ScriptName = "BindEventByTiming"), Category="TM|TimeBasedEvents")
	static FTMTimerHandle K2_BindEventByTime(const UObject* WorldContextObject, FTimespan InitialTime, FTMTimerDynamicDelegate Event, ETMTimePeriod Period = ETMTimePeriod::Day);
	UFUNCTION(BlueprintCallable, Meta=(DisplayName = "Bind Event by DateTime", WorldContext="WorldContextObject", ScriptName = "BindEventByDateTime"), Category="TM|TimeBasedEvents")
	static FTMTimerHandle K2_BindEventByDateAndTime(const UObject* WorldContextObject, FDateTime DateTime, FTMTimerDynamicDelegate Event);
	
	UFUNCTION(BlueprintPure, Category = "TM|Conversions")
	static FString TimeToString(FTimespan Time);
	UFUNCTION(BlueprintPure, Category = "TM|Conversions")
	static FString DateToString(FDateTime Date);
	UFUNCTION(BlueprintPure, Category = "TM|Conversions")
	static int64 ToInt64(FTimespan Source);
	UFUNCTION(BlueprintPure, Category = "TM|Conversions")
	static bool ParseTimeFromString(const FString& TimespanString, FTimespan& OutTimespan);
	UFUNCTION(BlueprintPure, Category = "TM|Conversions")
	static bool ParseDateFromString(const FString& DateString, FDateTime& OutDate);
	UFUNCTION(BlueprintPure, Meta=(DisplayName = "Reak Time to Game", WorldContext="WorldContextObject", ScriptName = "RealTimeToGame"), Category = "TM|Conversions")
	static FTimespan K2_RealTimeFloatToGameTimestamp(const UObject* WorldContextObject, float RealTime);
	UFUNCTION(BlueprintPure, Meta=(DisplayName = "Game Time to Real", WorldContext="WorldContextObject", ScriptName = "GameTimeToReal"), Category = "TM|Conversions")
	static float K2_GameTimestampToRealTimeFloat(const UObject* WorldContextObject, FTimespan GameTime);	
	
	/**Clamps time in 00:00:00 - 23:59:59 format (w/o negative or positive values)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TM|Calculations")
	static FTimespan ClampTime(UPARAM(ref) FTimespan& Value);	
};
