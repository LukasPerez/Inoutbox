#include "StatusComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	CurrentHealth = MaxHealth;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsServer())
	{
		CurrentHealth = MaxHealth;

		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}
}

bool UStatusComponent::IsServer() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, CurrentHealth);
}

void UStatusComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UStatusComponent::ApplyDamage_Internal(float DamageAmount)
{
	if (DamageAmount <= 0.f || !IsAlive())
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);
	
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (!IsAlive())
	{
		//ToDo
		
		
	}
}

void UStatusComponent::ServerApplyDamage_Implementation(float DamageAmount)
{
	ApplyDamage_Internal(DamageAmount);
}

void UStatusComponent::ApplyDamage(float DamageAmount)
{
	if (IsServer())
	{
		ApplyDamage_Internal(DamageAmount);
	}
	else
	{
		ServerApplyDamage(DamageAmount);
	}
}
