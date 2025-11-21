// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "MainHUDWidget.generated.h"

class UPanelWidget;
class UStatsPanelWidget;
class AInoutboxCharacter;

/**
 * 
 */
UCLASS()
class INOUTBOX_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="HUD")
	void InitializeForCharacter(AInoutboxCharacter* InCharacter);
	
	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

protected:

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UPanelWidget* CenterPanel;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UStatsPanelWidget* StatsPanel;

	UPROPERTY(BlueprintReadOnly, Category="HUD")
	TWeakObjectPtr<AInoutboxCharacter> CachedCharacter;
};
