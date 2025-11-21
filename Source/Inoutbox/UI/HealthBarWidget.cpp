// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

#include "Character/InoutboxCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"

void UHealthBarWidget::SetHealthPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UHealthBarWidget::SetCooldown(float InCooldownEndTime, float Duration)
{
    CooldownDuration     = FMath::Max(Duration, KINDA_SMALL_NUMBER);
    CooldownEndTime_World = InCooldownEndTime;
    bCooldownActive      = (CooldownEndTime_World > 0.f);

    if (CooldownBar)
    {
        CooldownBar->SetPercent(bCooldownActive ? 1.f : 0.f);
    }
}

void UHealthBarWidget::SetCooldownPercent(float Percent)
{
    if (CooldownBar)
    {
        CooldownBar->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
    }
}

void UHealthBarWidget::SetStateText(const FText& InText)
{
    if (StateText)
    {
        StateText->SetText(InText);
    }
}

void UHealthBarWidget::SetIsPlayerBar(bool bInIsPlayerBar)
{
    bIsPlayerBar = bInIsPlayerBar;
    UpdateVisibilityByMode();
}

void UHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    UpdateVisibilityByMode();
}

void UHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateVisibilityByMode();
    
    if (!bCooldownActive || CooldownDuration <= 0.f || !CooldownBar)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const float Now       = World->GetTimeSeconds();
    float Remaining       = CooldownEndTime_World - Now;

    if (Remaining <= 0.f)
    {
        Remaining      = 0.f;
        bCooldownActive = false;
    }

    const float Percent = FMath::Clamp(Remaining / CooldownDuration, 0.f, 1.f);
    CooldownBar->SetPercent(Percent);
}

void UHealthBarWidget::UpdateVisibilityByMode()
{
    
    /*
    if (bIsPlayerBar)
    {
        if (HealthBar)
        {
            HealthBar->SetVisibility(ESlateVisibility::Visible);
        }
        if (CooldownBar)
        {
            CooldownBar->SetVisibility(ESlateVisibility::Collapsed);
        }
        if (StateText)
        {
            StateText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
    else
    {
        if (HealthBar)
        {
            HealthBar->SetVisibility(ESlateVisibility::Collapsed);
        }
        if (CooldownBar)
        {
            CooldownBar->SetVisibility(ESlateVisibility::Visible);
        }
        if (StateText)
        {
            StateText->SetVisibility(ESlateVisibility::Visible);
        }
    }*/
}
