// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WayPoint.generated.h"

UCLASS()
class INOUTBOX_API AWayPoint : public AActor
{
	GENERATED_BODY()

public:
	
	AWayPoint();

protected:
	
	virtual void BeginPlay() override;

};
