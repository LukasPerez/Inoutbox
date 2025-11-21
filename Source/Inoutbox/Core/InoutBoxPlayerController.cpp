// Fill out your copyright notice in the Description page of Project Settings.


#include "InoutBoxPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/InoutboxCharacter.h"
#include "UI/MainHUDWidget.h"


AInoutBoxPlayerController::AInoutBoxPlayerController()
{
	bReplicates = true;
}

void AInoutBoxPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && MainHUDClass)
	{
		MainHUDInstance = CreateWidget<UMainHUDWidget>(this, MainHUDClass);
		if (MainHUDInstance)
		{
			MainHUDInstance->AddToViewport();
			
			if (AInoutboxCharacter* InoutCharacter = Cast<AInoutboxCharacter>(GetPawn()))
			{
				MainHUDInstance->InitializeForCharacter(InoutCharacter);
			}
		}
	}
}

void AInoutBoxPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (MainHUDInstance)
	{
		if (AInoutboxCharacter* InoutCharacter = Cast<AInoutboxCharacter>(InPawn))
		{
			MainHUDInstance->InitializeForCharacter(InoutCharacter);
		}
	}
}

void AInoutBoxPlayerController::OnControlledCharacterHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (MainHUDInstance)
	{
		MainHUDInstance->UpdateHealth(CurrentHealth, MaxHealth);
	}
}
