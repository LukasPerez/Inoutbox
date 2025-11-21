
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StalkerTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "UObject/EnumProperty.h"
#include "StalkerAIController.generated.h"

class UStalkerStateComponent;
class AInoutboxCharacter;
class AActor;
class AWayPoint;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class INOUTBOX_API AStalkerAIController : public AAIController
{
	GENERATED_BODY()

public:
	AStalkerAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentTarget, BlueprintReadOnly, Category="AI|Target")
	AActor* CurrentTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Config")
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Config")
	float LoseTargetTime = 3.0f; 
	
	void SetTarget(AActor* NewTarget);

protected:

	UPROPERTY(Transient)
	AInoutboxCharacter* ControlledStalker;

	UPROPERTY(Transient)
	UStalkerStateComponent* StalkerStateComponent;
	//------- State Machine

	UPROPERTY(ReplicatedUsing=OnRep_StalkerState, BlueprintReadOnly, Category="AI|State")
	EStalkerState StalkerState;
	
	UFUNCTION(BlueprintCallable)
	void SetStalkerState(EStalkerState NewState);

	void OnEnterState(EStalkerState NewState);
	void OnExitState(EStalkerState OldState);

	void TickInit(float DeltaSeconds);
	void TickState(float DeltaSeconds);
	void TickIdle(float DeltaSeconds);
	void TickPatrol(float DeltaSeconds);
	void TickChase(float DeltaSeconds);
	void TickAttack(float DeltaSeconds);
	void TickCooldown(float DeltaSeconds);
	
	/** RepNotify callbacks */

	UFUNCTION()
	void OnRep_StalkerState();//Deprecated

	UFUNCTION()
	void OnRep_CurrentTarget();

	// PATROL 
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="AI|Patrol")
	TArray<AWayPoint*> PatrolPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Patrol")
	float PatrolAcceptanceRadius = 120.f;
	
	int32 CurrentPatrolIndex = 0;
	
	AWayPoint* GetCurrentPatrolPoint() const;
	
	void InitPatrolRouteFromPawn(APawn* InPawn);
	
	void AdvancePatrolIndex();
	
	void StartPatrolMove();

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
 // ---------- PERCEPTION ----------
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|Perception", meta=(AllowPrivateAccess="true"))
    UAIPerceptionComponent* AIPerceptionComp;

	float TimeSinceLastTargetUpdate = 0.f;
	
    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY()
    UAISenseConfig_Hearing* HearingConfig;

    UFUNCTION()
    void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
    void SetCurrentTarget(AActor* NewTarget);
    void ClearCurrentTarget();


//-----------Attack

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Combat")
	float AttackCooldown = 5.0f; 

	bool bAttackOnCooldown = false;

	FTimerHandle AttackCooldownTimerHandle;
	
	bool IsTargetInAttackRange() const;
	
	void TryAttack();
	void StartAttackCooldown();
	void OnAttackCooldownFinished();
	
	//-------- Enums Helpers

public:
	
	static FString StalkerStateToString(EStalkerState State)
	{
		if (const UEnum* Enum = StaticEnum<EStalkerState>())
		{
			return Enum->GetNameStringByValue(static_cast<int64>(State));
		}

		return TEXT("Unknown");
	}
	
};
