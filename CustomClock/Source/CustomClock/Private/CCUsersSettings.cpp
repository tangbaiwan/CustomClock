// Copyright Epic Games, Inc. All Rights Reserved.

#include "CCUsersSettings.h"

UCCUsersSettings::FOnUpdateSettings UCCUsersSettings::OnSettingsChange;

UCCUsersSettings::UCCUsersSettings()
{
	CategoryName = TEXT("Project");
	SectionName = TEXT("BXS_Clock");
}

void UCCUsersSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	OnSettingsChange.Broadcast();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}