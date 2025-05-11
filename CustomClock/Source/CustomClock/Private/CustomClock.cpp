// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomClock.h"
#include "Engine.h"
#define LOCTEXT_NAMESPACE "FCustomClockModule"


void FCustomClockModule::StartupModule()
{
	UCCUsersSettings::OnSettingsChange.AddRaw(this, &FCustomClockModule::OnSettingsChange);
	TestOutTime();
}

void FCustomClockModule::ShutdownModule()
{
#if WITH_EDITOR
	UCCUsersSettings::OnSettingsChange.RemoveAll(this);
#endif
}
void FCustomClockModule::TestOutTime()
{
    AsyncTask(ENamedThreads::GameThread, [&]()
        {
            auto Func = [&]() {
				CurTime = FDateTime::Now().ToString(TEXT("%H-%M-%S"));
				CurTimeList.Empty();
				CurTime.ParseIntoArray(CurTimeList, TEXT("-"));

				CurIntArray.Empty();
				for (const FString& Str : CurTimeList)
				{
					CurIntArray.Add(FCString::Atoi(*Str));
				}
				UCCUsersSettings::Get()->CallClock(CurIntArray);
                };
            GEditor->GetTimerManager()->SetTimer(TimerHandle, Func, 1.0f, true);
        });
}

void FCustomClockModule::OnSettingsChange()
{
	if (TimerHandle.IsValid())
	{
		GEditor->GetTimerManager()->ClearTimer(TimerHandle);
	}
	for (auto& CurClockInfo : UCCUsersSettings::Get()->ClockList)
	{
		CurClockInfo.bEnd = false;
	}
	TestOutTime();
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomClockModule, CustomClock)