// StalkerAIController.cpp

#include "AI/StalkerAIController.h"

#include "PatrolRouteComponent.h"
#include "WayPoint.h"
#include "Net/UnrealNetwork.h"
#include "Character/InoutboxCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "StalkerStateComponent.h"
#include "Components/WidgetComponent.h"
#include "NavMesh/RecastNavMesh.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Status/StatusComponent.h"
#include "UI/HealthBarWidget.h"

AStalkerAIController::AStalkerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bWantsPlayerState = false;

	StalkerState = EStalkerState::Init;
	
	// --- PERCEPTION ---

	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	SetPerceptionComponent(*AIPerceptionComp);

	// Vista
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2500.f;
	SightConfig->LoseSightRadius = 2800.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->SetMaxAge(5.f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 2000.f;
	//HearingConfig->LoSHearingRange = 2500.f;
	HearingConfig->SetMaxAge(5.f);

	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AStalkerAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&AStalkerAIController::HandleTargetPerceptionUpdated
		);
	}
	
	if (HasAuthority())
	{
		SetStalkerState(EStalkerState::Init);
	}

}

void AStalkerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledStalker = Cast<AInoutboxCharacter>(InPawn);

	InitPatrolRouteFromPawn(InPawn);

	if (HasAuthority() && StalkerState == EStalkerState::Idle)
	{
		SetStalkerState(EStalkerState::Patrol);
	}

	if(ControlledStalker)
	{
		StalkerStateComponent = ControlledStalker->FindComponentByClass<UStalkerStateComponent>();
	}
}

void AStalkerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!HasAuthority())
	{
		return;
	}

	TickState(DeltaSeconds);
}

void AStalkerAIController::TickState(float DeltaSeconds)
{
	switch (StalkerState)
	{
	case EStalkerState::Init:
		TickInit(DeltaSeconds);
		break;
	case EStalkerState::Idle:
		TickIdle(DeltaSeconds);
		break;
	case EStalkerState::Patrol:
		TickPatrol(DeltaSeconds);
		break;
	case EStalkerState::Chase:
		TickChase(DeltaSeconds);
		break;
	case EStalkerState::Attack:
		TickAttack(DeltaSeconds);
		break;
	case EStalkerState::Cooldown:
		TickCooldown(DeltaSeconds);
		break;
	default:
		break;
	}
}

void AStalkerAIController::TickIdle(float DeltaSeconds)
{

	
}

void AStalkerAIController::TickPatrol(float DeltaSeconds)
{
	//Found Something
	if (CurrentTarget)
	{
		SetStalkerState(EStalkerState::Chase);
	}
}

void AStalkerAIController::TickChase(float DeltaSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!CurrentTarget)
	{
		SetStalkerState(EStalkerState::Patrol);
		return;
	}

	TimeSinceLastTargetUpdate += DeltaSeconds;
	if (TimeSinceLastTargetUpdate >= LoseTargetTime)
	{
		CurrentTarget = nullptr;
		SetStalkerState(EStalkerState::Patrol);
		return;
	}

	const bool bInAttackRange = IsTargetInAttackRange();
	
	if (bInAttackRange && !bAttackOnCooldown)
	{
		SetStalkerState(EStalkerState::Attack);
		return;
	}
	
	MoveToActor(CurrentTarget, AttackRange * 0.8f, true, true, true, 0, true);
}


void AStalkerAIController::TickAttack(float DeltaSeconds)
{
	// ToDo. maybe later with anims
	SetStalkerState(EStalkerState::Cooldown);
}

void AStalkerAIController::TickCooldown(float DeltaSeconds)
{
	
}

bool AStalkerAIController::IsTargetInAttackRange() const
{
	if (!ControlledStalker || !CurrentTarget)
	{
		return false;
	}

	const float DistSq = FVector::DistSquared(ControlledStalker->GetActorLocation(), CurrentTarget->GetActorLocation());
	return DistSq <= FMath::Square(AttackRange);
}

void AStalkerAIController::SetStalkerState(EStalkerState NewState)
{
	if (!HasAuthority())
	{
		return;
	}

	if (StalkerState == NewState)
	{
		return;
	}

	const EStalkerState OldState = StalkerState;

	OnExitState(OldState);
	StalkerState = NewState;

	UE_LOG(LogTemp, Log,
		TEXT("[StalkerAI] %s state: %s -> %s"),
		*GetName(),
		*StalkerStateToString(OldState),
		*StalkerStateToString(NewState));
	
	OnEnterState(NewState);

	if(StalkerStateComponent)
	{
		StalkerStateComponent->SetStalkerState(NewState);
	}
	//OnRep_StalkerState();
}

void AStalkerAIController::OnEnterState(EStalkerState NewState)
{
	switch (NewState)
	{
	case EStalkerState::Init:

		StopMovement();
		break;
	case EStalkerState::Idle:

		StopMovement();
		break;
	case EStalkerState::Patrol:

		StartPatrolMove();
		break;
	case EStalkerState::Chase:

		if (CurrentTarget)
		{
			MoveToActor(CurrentTarget, 100.f);
		}
		TimeSinceLastTargetUpdate = 0.f;
		break;
	case EStalkerState::Attack:

		StopMovement();
		TryAttack(); 
		break;
	case EStalkerState::Cooldown:

		break;
	default:
		break;
	}
}

void AStalkerAIController::OnExitState(EStalkerState OldState)
{
	switch (OldState)
	{
	case EStalkerState::Attack:

		break;
	case EStalkerState::Cooldown:

		GetWorldTimerManager().ClearTimer(AttackCooldownTimerHandle);
		bAttackOnCooldown = false;
		break;
	default:
		break;
	}
}

void AStalkerAIController::TickInit(float DeltaSeconds)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GetPawn())
	{
		return;
	}

	if (PatrolPoints.Num() == 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		return;
	}
	
	// ToDo: 5.7 changes some navmesh calls, Improve here
	
	AWayPoint* FirstPoint = PatrolPoints.IsValidIndex(0) ? PatrolPoints[0] : nullptr;
	if (!FirstPoint)
	{
		return;
	}

	const FVector TargetLocation = FirstPoint->GetActorLocation();
	const FVector QueryExtent(200.f, 200.f, 300.f);
	FNavLocation ProjectedLocation;

	const bool bProjected = NavSys->ProjectPointToNavigation(
		TargetLocation,
		ProjectedLocation,
		QueryExtent
	);

	if (!bProjected)
	{
		return;
	}

	//Nav Mesh Up
	UE_LOG(LogTemp, Log, TEXT("NavMesh built, starting Patrol for %s"), *GetName());
	SetStalkerState(EStalkerState::Patrol);
}

// --- Target ---

void AStalkerAIController::SetTarget(AActor* NewTarget)
{
	if (!HasAuthority())
	{
		return;
	}

	if (CurrentTarget == NewTarget)
	{
		return;
	}

	CurrentTarget = NewTarget;
	TimeSinceLastTargetUpdate = 0.f;

	OnRep_CurrentTarget();

	if (CurrentTarget && (StalkerState == EStalkerState::Idle || StalkerState == EStalkerState::Patrol))
	{
		SetStalkerState(EStalkerState::Chase);
	}
}


void AStalkerAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStalkerAIController, StalkerState);
	DOREPLIFETIME(AStalkerAIController, CurrentTarget);
}

void AStalkerAIController::OnRep_StalkerState()
{

}


void AStalkerAIController::OnRep_CurrentTarget()
{
	
}

AWayPoint* AStalkerAIController::GetCurrentPatrolPoint() const
{
    if (PatrolPoints.Num() == 0)
    {
        return nullptr;
    }

    if (!PatrolPoints.IsValidIndex(CurrentPatrolIndex))
    {
        return PatrolPoints[0];
    }

    return PatrolPoints[CurrentPatrolIndex];
}

void AStalkerAIController::InitPatrolRouteFromPawn(APawn* InPawn)
{
	PatrolPoints.Reset();

	if (!InPawn)
	{
		return;
	}

	if (UPatrolRouteComponent* PatrolComp = InPawn->FindComponentByClass<UPatrolRouteComponent>())
	{
		const TArray<AWayPoint*>& RoutePoints = PatrolComp->GetPatrolPoints();
		PatrolPoints = RoutePoints; 

		
	}
}

void AStalkerAIController::AdvancePatrolIndex()
{
    if (PatrolPoints.Num() == 0)
    {
        return;
    }

    CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
}

void AStalkerAIController::StartPatrolMove()
{
	if (!HasAuthority() || StalkerState != EStalkerState::Patrol)
	{
		return;
	}

	AWayPoint* PatrolPoint = GetCurrentPatrolPoint();
	if (!PatrolPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartPatrolMove: no PatrolPoint"));
		StopMovement();
		return;
	}

	const FVector TargetLocation = PatrolPoint->GetActorLocation();

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartPatrolMove: NO NavSys"));
		StopMovement();
		return;
	}

	FNavLocation ProjectedLocation;
	const FVector QueryExtent(50.f, 50.f, 200.f); 

	const bool bProjected = NavSys->ProjectPointToNavigation(
		TargetLocation,
		ProjectedLocation,
		QueryExtent
	);

	if (!bProjected)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("StartPatrolMove: ProjectPointToNavigation FAILED for %s at %s"),
			*PatrolPoint->GetName(),
			*TargetLocation.ToString());

		StopMovement();
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("StartPatrolMove: moving to projected %s (from %s)"),
		*ProjectedLocation.Location.ToString(),
		*TargetLocation.ToString());

	MoveToLocation(ProjectedLocation.Location, PatrolAcceptanceRadius, true, true, true, false);
}

void AStalkerAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!HasAuthority())
	{
		return;
	}

	// Only in Patrol
	if (StalkerState != EStalkerState::Patrol)
	{
		return;
	}

	// Next Waypoint
	if (Result.IsSuccess())
	{
		AdvancePatrolIndex();
		StartPatrolMove();
	}
	else if (Result.Code == EPathFollowingResult::Aborted)
	{
		if (!CurrentTarget)
		{
			StartPatrolMove();
		}
	}
	else
	{
		// Fail
		UE_LOG(LogTemp, Warning,
			TEXT("Patrol move failed for %s with result code %d"),
			*GetName(), static_cast<int32>(Result.Code));

		
		StopMovement();
		SetStalkerState(EStalkerState::Idle);
	}
	
}

void AStalkerAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (CurrentTarget == NewTarget)
	{
		return;
	}

	CurrentTarget = NewTarget;

	if (HasAuthority())
	{
		if (CurrentTarget)
		{
			
			if (StalkerState == EStalkerState::Patrol || StalkerState == EStalkerState::Idle)
			{
				SetStalkerState(EStalkerState::Chase);
			}
		}
		else
		{
			
			if (StalkerState == EStalkerState::Chase ||	StalkerState == EStalkerState::Attack || StalkerState == EStalkerState::Cooldown)
			{
				SetStalkerState(EStalkerState::Patrol);
			}
		}
	}
	
}

void AStalkerAIController::ClearCurrentTarget()
{
	SetCurrentTarget(nullptr);
}

void AStalkerAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasAuthority() || !Actor)
	{
		return;
	}

	AInoutboxCharacter* SensedChar = Cast<AInoutboxCharacter>(Actor);
	if (!SensedChar || !SensedChar->IsPlayerControlled())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{

		SetCurrentTarget(SensedChar);

	}
	else
	{
		
		if (CurrentTarget == SensedChar)
		{
			ClearCurrentTarget();
		}
	}
}

void AStalkerAIController::TryAttack()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ControlledStalker || !CurrentTarget)
	{
		SetStalkerState(EStalkerState::Patrol);
		return;
	}

	if (!IsTargetInAttackRange())
	{
		SetStalkerState(EStalkerState::Chase);
		return;
	}

	if (AInoutboxCharacter* TargetChar = Cast<AInoutboxCharacter>(CurrentTarget))
	{
		if (UStatusComponent* Status = TargetChar->FindComponentByClass<UStatusComponent>())
		{
			if(!bAttackOnCooldown)
			{
				Status->ApplyDamage(10.f);
			}
			
		}
	}

	StartAttackCooldown();

	SetStalkerState(EStalkerState::Cooldown);
}

void AStalkerAIController::StartAttackCooldown()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bAttackOnCooldown)
	{
		return;
	}

	bAttackOnCooldown = true;

	GetWorldTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&AStalkerAIController::OnAttackCooldownFinished,
		AttackCooldown,
		false
	);
}

void AStalkerAIController::OnAttackCooldownFinished()
{
	if (!HasAuthority())
	{
		return;
	}

	bAttackOnCooldown = false;

	if (CurrentTarget)
	{
		SetStalkerState(EStalkerState::Chase);
	}
	else
	{
		SetStalkerState(EStalkerState::Patrol);
	}
}

