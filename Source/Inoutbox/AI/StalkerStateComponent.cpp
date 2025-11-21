// StalkerStateComponent.cpp

#include "StalkerStateComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "UI/HealthBarWidget.h"
#include "Character/InoutboxCharacter.h"
#include "StalkerAIController.h"
#include "Character/InoutboxCharacter.h"
#include "UI/HealthBarWidget.h"

UStalkerStateComponent::UStalkerStateComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UStalkerStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UStalkerStateComponent, StalkerState);
    DOREPLIFETIME(UStalkerStateComponent, AttackCooldownEndTime);
}

void UStalkerStateComponent::SetStalkerState(EStalkerState NewState)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    if (StalkerState == NewState)
    {
        return;
    }

    StalkerState = NewState;
    
    OnRep_StalkerState();
}

void UStalkerStateComponent::StartAttackCooldown(float Duration)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    AttackCooldownDuration = Duration;
    AttackCooldownEndTime  = GetWorld()->GetTimeSeconds() + Duration;

    OnRep_AttackCooldown();
}

void UStalkerStateComponent::OnRep_StalkerState()
{
    UpdateWidget();
}

void UStalkerStateComponent::OnRep_AttackCooldown()
{
    UpdateWidget();
}

void UStalkerStateComponent::UpdateWidget()
{
    AInoutboxCharacter* OwnerChar = Cast<AInoutboxCharacter>(GetOwner());
    if (!OwnerChar || !OwnerChar->HealthBarComponent)
    {
        return;
    }

    UHealthBarWidget* Widget = Cast<UHealthBarWidget>(OwnerChar->HealthBarComponent->GetUserWidgetObject());
    if (!Widget)
    {
        return;
    }
    
    const bool bIsPlayer = OwnerChar->GetController() && OwnerChar->GetController()->IsPlayerController();
    if (bIsPlayer)
    {
        return;
    }
    
    Widget->SetStateText(
        FText::FromString(AStalkerAIController::StalkerStateToString(StalkerState))
    );

    if (AttackCooldownEndTime > 0.f && AttackCooldownDuration > 0.f)
    {
        Widget->SetCooldown(AttackCooldownEndTime, AttackCooldownDuration);
    }
}
