// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "StatsPanelWidget.generated.h"

class UProgressBar;

UCLASS()
class INOUTBOX_API UStatsPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="Stats")
	void SetHealth(float InCurrent, float InMax);

protected:

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* HealthTextBlock;

	UPROPERTY(BlueprintReadOnly, Category="Stats")
	float CurrentHealth = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="Stats")
	float MaxHealth = 100.f;
	
};
