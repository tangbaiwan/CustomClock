// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CCUsersSettings.h"
#include "Modules/ModuleManager.h"

class FCustomClockModule : public IModuleInterface
{

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	FTimerHandle TimerHandle;
	void TestOutTime();
	void OnSettingsChange();

public:
	FString CurTime;
	TArray<FString> CurTimeList;
	TArray<int> CurIntArray;
};
