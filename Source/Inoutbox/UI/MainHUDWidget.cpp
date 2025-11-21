// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"

#include "StatsPanelWidget.h"
#include "Character/InoutboxCharacter.h"

void UMainHUDWidget::InitializeForCharacter(AInoutboxCharacter* InCharacter)
{
	CachedCharacter = InCharacter;
	
	if (StatsPanel && InCharacter)
	{
		StatsPanel->SetHealth(
			InCharacter->GetHealth(),
			InCharacter->GetHealth());
	}
}

void UMainHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (StatsPanel)
	{
		StatsPanel->SetHealth(CurrentHealth, MaxHealth);
	}
}