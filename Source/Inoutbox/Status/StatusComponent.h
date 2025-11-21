#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INOUTBOX_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatusComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth, VisibleAnywhere, BlueprintReadOnly, Category="Health")
	float CurrentHealth = 0.f;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void ApplyDamage_Internal(float DamageAmount);
	
	bool IsServer() const;

public:
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHealthChangedSignature OnHealthChanged;

	//----- Getters

	UFUNCTION(BlueprintCallable, Category="Health")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category="Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category="Health")
	float GetHealthPercent() const { return (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f; }

	UFUNCTION(BlueprintCallable, Category="Health")
	bool IsAlive() const { return CurrentHealth > 0.f; }

	//--- RPC

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category="Health")
	void ApplyDamage(float DamageAmount);
};
