// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"


class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class INOUTBOX_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="HealthBar")
	void SetHealthPercent(float Percent);

	UFUNCTION(BlueprintCallable, Category="HealthBar")
	void SetCooldown(float CooldownEndTime, float Duration);
	
	UFUNCTION(BlueprintCallable, Category="HealthBar")
	void SetCooldownPercent(float Percent);
	
	UFUNCTION(BlueprintCallable, Category="HealthBar")
	void SetStateText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category="HealthBar")
	void SetIsPlayerBar(bool bInIsPlayerBar);

protected:

	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UProgressBar* CooldownBar;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* StateText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HealthBar", meta=(ExposeOnSpawn="true"))
	bool bIsPlayerBar = true;
	
	float CooldownEndTime_World = 0.f;
	float CooldownDuration = 0.f;
	bool bCooldownActive = false;

	//Deprecated
	void UpdateVisibilityByMode();
};
