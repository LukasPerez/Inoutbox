// PatrolRouteComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PatrolRouteComponent.generated.h"

class AWayPoint;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INOUTBOX_API UPatrolRouteComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPatrolRouteComponent();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Patrol Route")
	TArray<AWayPoint*> PatrolPoints;

public:
	
	UFUNCTION(BlueprintCallable, Category="Patrol Route")
	const TArray<AWayPoint*>& GetPatrolPoints() const { return PatrolPoints; }
	
	UFUNCTION(BlueprintCallable, Category="Patrol Route")
	bool HasPatrolRoute() const { return PatrolPoints.Num() > 0; }
	
};