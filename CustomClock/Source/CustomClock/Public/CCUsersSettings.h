// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "UObject/ObjectMacros.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "IPythonScriptPlugin.h"
#include "CCUsersSettings.generated.h"

UENUM()
enum class ClockMode : uint8
{
	MessageDialog = 0,
	notification,
	ExecScript,
	None,
};

UENUM()
enum class TimeMode : uint8
{
	Once = 0,
	EveryHours,
	EveryMinter,
	EverySecond,
};

USTRUCT(BlueprintType)
struct FCafeTimeDate
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClockConfig")
	FString CurClockName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClockConfig")
	ClockMode CurClockMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClockConfig")
	TimeMode CurTimeMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time", meta = (ClampMin = "0", ClampMax = "23", EditCondition = "!(CurTimeMode!=TimeMode::Once)"))
	int Hours;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time", meta = (ClampMin = "0", ClampMax = "59", EditCondition = "CurTimeMode!=TimeMode::EveryMinter && CurTimeMode!=TimeMode::EverySecond"))
	int Minter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time", meta = (ClampMin = "0", ClampMax = "59", EditCondition = "CurTimeMode!=TimeMode::EverySecond"))
	int Second;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Date Time", meta = (ClampMin = "0", ClampMax = "59", Visibility = "CurClockMode==ClockMode::ExecScript"))
	FString Script;

	bool bEnd = false;
};


UCLASS(config = BXS_Clock, meta = (DisplayName = "BXS_Clock"))
class UCCUsersSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UCCUsersSettings();
#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE(FOnUpdateSettings);
	static FOnUpdateSettings OnSettingsChange;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	static UCCUsersSettings* Get() { return GetMutableDefault<UCCUsersSettings>(); }

	static void OpenDiago(FString Content)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Content));
		return;
	}

	static void ExecScript(FString TitleContent,FString Script)
	{
		if (Script.StartsWith("PY "))
		{
			FString RealFunctionScript;
			if (Script.Split(TEXT(" "), nullptr, &RealFunctionScript))
			{
				IPythonScriptPlugin::Get()->ExecPythonCommand(*RealFunctionScript);
				return;
			}
		}
		IPythonScriptPlugin::Get()->ExecPythonCommand(TEXT("import unreal"));
		IPythonScriptPlugin::Get()->ExecPythonCommand(TEXT("EUW=unreal.new_object(unreal.EditorAssetLibrary.load_blueprint_class(\"/Game/Developers/baixiaosheng/EUW_CallFunction\"))"));
		FString RealFunctionScript = TEXT("EUW.call_method(\"") + Script + TEXT("\",())");
		IPythonScriptPlugin::Get()->ExecPythonCommand(*RealFunctionScript);
	}

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "BXS|NotificationHelpers")
	static void CreateEditorNotification(const FText Message,
		const float fadeInDuration = 0.5f, const float expireDuration = 8.0f,
		const float fadeOutDuration = 3.0f, const bool useThrobber = false,
		const bool useSuccessFailIcons = true, const bool useLargeFont = true,
		const bool fireAndForget = true, const bool allowThrottleWhenFrameRateIsLow = false)
	{
		FNotificationInfo Info(Message);
		Info.FadeInDuration = fadeInDuration;
		Info.ExpireDuration = expireDuration;
		Info.FadeOutDuration = fadeOutDuration;
		Info.bUseThrobber = useThrobber;
		Info.bUseSuccessFailIcons = useSuccessFailIcons;
		Info.bUseLargeFont = useLargeFont;
		Info.bFireAndForget = fireAndForget;
		Info.bAllowThrottleWhenFrameRateIsLow = allowThrottleWhenFrameRateIsLow;
		auto NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
		NotificationItem->SetCompletionState(SNotificationItem::ECompletionState::CS_Success);
		NotificationItem->ExpireAndFadeout();
	}

	void CallClock(TArray<int>& CurIntArray)
	{
		for (auto& CurClockInfo: ClockList)
		{
			int CurTimeModeInt = static_cast<int>(CurClockInfo.CurTimeMode);
			if (CurClockInfo.bEnd && CurTimeModeInt==0)
			{
				continue;
			}
			if (CurClockInfo.Hours == CurIntArray[0] || CurTimeModeInt >0)
			{
				if (CurClockInfo.Minter == CurIntArray[1] || CurTimeModeInt >1)
				{
					if (CurClockInfo.Second == CurIntArray[2] || CurTimeModeInt == 3)
					{
						if (CurClockInfo.CurTimeMode == TimeMode::Once)
						{
							CurClockInfo.bEnd = true;
						}
						FString CurDialog = FString::Printf(TEXT("现在是: \n%d点%d分%d秒                    "), CurIntArray[0], CurIntArray[1], CurIntArray[2]) + "\n" + CurClockInfo.CurClockName;
						if (CurClockInfo.CurClockMode== ClockMode::notification)
						{
							CreateEditorNotification(FText::FromString(CurDialog));
						}
						else if (CurClockInfo.CurClockMode == ClockMode::MessageDialog)
						{
							OpenDiago(CurDialog);
						}
						else if (CurClockInfo.CurClockMode == ClockMode::ExecScript)
						{
							ExecScript(CurDialog, CurClockInfo.Script);
						}
					}
				}
			}
		}
	}

#endif // WITH_EDITOR
	UPROPERTY(config, EditAnywhere, Category = "闹钟设置")
	TArray< FCafeTimeDate> ClockList;

	TMap<FString,TArray<int>> AllIntArrayMap;
};
