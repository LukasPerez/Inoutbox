// PatrolRouteComponent.cpp

#include "PatrolRouteComponent.h"
#include "WayPoint.h"

UPatrolRouteComponent::UPatrolRouteComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(false);
}

void UPatrolRouteComponent::BeginPlay()
{
	Super::BeginPlay();


}