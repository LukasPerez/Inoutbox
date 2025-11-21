// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatsPanelWidget.h"
#include "Components/ProgressBar.h"

void UStatsPanelWidget::SetHealth(float InCurrent, float InMax)
{
	CurrentHealth = InCurrent;
	MaxHealth     = InMax;

	const float Percent = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;

	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}

	if(HealthTextBlock)
	{
		const FString HpString = FString::Printf(TEXT("HP: %.0f / %.0f"), CurrentHealth, MaxHealth);
		HealthTextBlock->SetText(FText::FromString(HpString));
	}
}