// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "InoutBoxPlayerController.generated.h"

class UMainHUDWidget;
/**
 * 
 */
UCLASS()
class INOUTBOX_API AInoutBoxPlayerController : public AALSPlayerController
{
	GENERATED_BODY()

public:

	AInoutBoxPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintCallable, Category="HUD")
	void OnControlledCharacterHealthChanged(float CurrentHealth, float MaxHealth);

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
	TSubclassOf<UMainHUDWidget> MainHUDClass;

	UPROPERTY()
	UMainHUDWidget* MainHUDInstance;
	
};
