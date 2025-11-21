// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/InoutboxCharacter.h"
#include "Components/WidgetComponent.h"
#include "Core/InoutBoxPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Hearing.h"
#include "Status/StatusComponent.h"
#include "UI/HealthBarWidget.h"


AInoutboxCharacter::AInoutboxCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComponent"));
	if (HealthBarComponent)
	{
		HealthBarComponent->SetupAttachment(GetRootComponent());
		HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);      
		HealthBarComponent->SetDrawAtDesiredSize(true);
		HealthBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); 
	}

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
}

void AInoutboxCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (StatusComponent)
	{
		StatusComponent->OnHealthChanged.AddDynamic(
			this, &AInoutboxCharacter::HandleHealthChanged);
		
		HandleHealthChanged(StatusComponent->GetHealth(), StatusComponent->GetMaxHealth());
	}

	if(HealthBarComponent)
	{
		UHealthBarWidget *MyHealthBar= Cast<UHealthBarWidget>( HealthBarComponent->GetUserWidgetObject());

		if(MyHealthBar)
		{
			MyHealthBar->SetIsPlayerBar(IsPlayerControlled());
		}
	}
}

void AInoutboxCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void AInoutboxCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Now in a component
	//DOREPLIFETIME(AInoutboxCharacter, CurrentHealth);
}

void AInoutboxCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);


	//ToDo, Improve this
	static const FName HeadSocketName(TEXT("head")); // "FP_Camera", "Head".

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		OutLocation = MeshComp->GetSocketLocation(HeadSocketName);
		OutLocation = GetActorLocation() + FVector(0.f, 0.f, 80.f);
	}
	else
	{
		OutLocation = GetActorLocation() + FVector(0.f, 0.f, 80.f); 
	}

	OutRotation = GetActorRotation();
}

void AInoutboxCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (!IsLocallyControlled())
	{
		return;
	}

	if (HealthBarComponent)
	{
		if (UHealthBarWidget* MyHealthBar = Cast<UHealthBarWidget>(HealthBarComponent->GetUserWidgetObject()))
		{
			MyHealthBar->SetIsPlayerBar(true);
		}
	}
}

void AInoutboxCharacter::UpdateHealthBarWidget()
{
	if (!HealthBarComponent)
	{
		return;
	}

	if (UUserWidget* UserWidget = HealthBarComponent->GetUserWidgetObject())
	{
		if (UHealthBarWidget* HealthWidget = Cast<UHealthBarWidget>(UserWidget))
		{
			HealthWidget->SetHealthPercent(GetHealthPercent());
		}
	}
}

float AInoutboxCharacter::GetHealth() const
{
	return StatusComponent ? StatusComponent->GetHealth() : 0.f;
}

float AInoutboxCharacter::GetMaxHealth() const
{
	return StatusComponent ? StatusComponent->GetMaxHealth() : 0.f;
}

float AInoutboxCharacter::GetHealthPercent() const
{
	return StatusComponent ? StatusComponent->GetHealthPercent() : 0.f;
}

bool AInoutboxCharacter::IsAlive() const
{
	return StatusComponent ? StatusComponent->IsAlive() : false;
}

void AInoutboxCharacter::ApplyDamage(float DamageAmount)
{
	if (StatusComponent)
	{
		StatusComponent->ApplyDamage(DamageAmount);
	}
}

void AInoutboxCharacter::HandleHealthChanged(float NewHealth, float MaxHealth)
{
	UpdateHealthBarWidget();

	if (AInoutBoxPlayerController* PC = Cast<AInoutBoxPlayerController>(GetController()))
	{
		PC->OnControlledCharacterHealthChanged(NewHealth, MaxHealth);
	}
}

void AInoutboxCharacter::ReportFootstepNoise()
{
	if (!HasAuthority())
		return;
	
	//----ALS--- ToDo:Improve this

	float Loudness = 0.5f;
	if(Stance == EALSStance::Crouching)
	{
		Loudness = 0.1f;
	}
	else if(Gait==EALSGait::Running)
	{
		Loudness = 1.0f;
	}
	else
	if(Gait==EALSGait::Running)
	{
		Loudness = 2.0f;
	}
	
	//const float Loudness = 1.0f;
	const float MaxRange = 1200.f;
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		Loudness,
		this,          // Instigator
		MaxRange,
		FName("Footstep")
	);
}

void AInoutboxCharacter::HandleALSFootstepNoise_Implementation(const FVector& Location, float Loudness)
{
	ReportFootstepNoise();

}