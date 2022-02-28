// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeControlled.h"

void ITimeControlled::SetCustomDilation_Implementation(float Value)
{
	AActor* SelfObject = Cast<AActor>(this);
	if(IsValid(SelfObject))
	{
		SelfObject->CustomTimeDilation = Value;
	}
}
