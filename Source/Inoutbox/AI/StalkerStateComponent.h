// StalkerStateComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StalkerTypes.h"          
#include "StalkerStateComponent.generated.h"

class UHealthBarWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INOUTBOX_API UStalkerStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStalkerStateComponent();
	
	UPROPERTY(ReplicatedUsing=OnRep_StalkerState, BlueprintReadOnly, Category="Stalker|State")
	EStalkerState StalkerState = EStalkerState::Init;
	
	UPROPERTY(ReplicatedUsing=OnRep_AttackCooldown, BlueprintReadOnly, Category="Stalker|Combat")
	float AttackCooldownEndTime = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category="Stalker|Combat")
	float AttackCooldownDuration = 0.f;
	
	UFUNCTION(BlueprintCallable, Category="Stalker|State")
	void SetStalkerState(EStalkerState NewState);

	UFUNCTION(BlueprintCallable, Category="Stalker|Combat")
	void StartAttackCooldown(float Duration);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_StalkerState();

	UFUNCTION()
	void OnRep_AttackCooldown();
	
	void UpdateWidget();
};
