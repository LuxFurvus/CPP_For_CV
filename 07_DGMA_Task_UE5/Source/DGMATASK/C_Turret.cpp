// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Turret.h"
#include "Net/UnrealNetwork.h"

//void AC_Turret::OnHealthChanged()
//{
//}

AC_Turret::AC_Turret()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	TurretBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBase"));
	TurretBase->SetupAttachment(RootComponent);
	//TurretBase->SetMobility(EComponentMobility::Static);

	TurretGun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretGun"));
	TurretGun->SetupAttachment(TurretBase);
	TurretGun->SetWorldLocation(FVector(0.0f, 0.0f, 50.0f));

	FirePoint = CreateDefaultSubobject<USphereComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(TurretGun);
	FirePoint->SetWorldLocation(FVector(0.0f, 230.0f, 70.0f));
	FirePoint->SetWorldRotation(FRotator(0.0f, 0.0f, 270.0f));
}

void AC_Turret::BeginPlay()
{
	Super::BeginPlay();	
}

void AC_Turret::SetTurretTraits(const F_TurretTraits& Traits)
{
	TurretTraits = Traits;
}

void AC_Turret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AC_Turret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_Turret, IsDead);
	DOREPLIFETIME(AC_Turret, TurretTraits);
}

//void AC_Turret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{	Super::SetupPlayerInputComponent(PlayerInputComponent);}
