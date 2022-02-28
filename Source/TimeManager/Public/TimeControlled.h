// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TimeControlled.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTimeControlled : public UInterface
{
	GENERATED_BODY()
};

class TIMEMANAGER_API ITimeControlled
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TimeControl")
	void SetCustomDilation(float NewValue);

	/**Default implementation of time management functions*/
	virtual void SetCustomDilation_Implementation(float Value);	
};
