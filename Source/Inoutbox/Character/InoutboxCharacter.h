// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSCharacter.h"
#include "Character/Animation/Notify/ALSFootstepNoiseInterface.h"
#include "InoutboxCharacter.generated.h"

class UStatusComponent;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class INOUTBOX_API AInoutboxCharacter : public AALSCharacter, public IALSFootstepNoiseInterface
{
	GENERATED_BODY()

public:

	AInoutboxCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
	virtual void OnRep_Controller() override;
	//--- Health

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* HealthBarComponent;
	
	void UpdateHealthBarWidget();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Status")
	UStatusComponent* StatusComponent;

public:
	//--- Getters
	
	UFUNCTION(BlueprintCallable, Category="Status")
	UStatusComponent* GetStatusComponent() const { return StatusComponent; }
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsAlive() const;

	//---- Damage
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float DamageAmount);

	protected:
	
	UFUNCTION()
	void HandleHealthChanged(float NewHealth, float MaxHealth);
	
	//---- Noise

	virtual void HandleALSFootstepNoise_Implementation(
	   const FVector& Location,
	   float Loudness) override;
	
	void ReportFootstepNoise();
	
};
