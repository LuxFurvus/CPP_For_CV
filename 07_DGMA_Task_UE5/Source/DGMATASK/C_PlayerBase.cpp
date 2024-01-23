// Fill out your copyright notice in the Description page of Project Settings.

#include "C_PlayerBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Containers/Array.h"
#include "TimerManager.h"
//////////////////////////////////////
#include "I_MenuConnect.h"
//////////////////////////////
#include "I_MarkBaseConnect.h"

// Sets default values
AC_PlayerBase::AC_PlayerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Platform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BasePlatform"));
	Platform->SetupAttachment(RootComponent);
	Platform->SetWorldLocation(FVector(0.0f, 0.0f, 0.0f));
	Platform->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	Platform->SetMobility(EComponentMobility::Static);

	Spire01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseSpire01"));
	Spire01->SetupAttachment(Platform);
	Spire01->SetWorldLocation(FVector(0.0f, 0.0f, 115.0f));
	Spire01->SetWorldRotation(FRotator(180.0f, 270.0f, 90.0f));
	Spire01->SetWorldScale3D(FVector(15.0f, 15.0f, 15.0f));
	Spire01->SetMobility(EComponentMobility::Static);

	Spire02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseSpire02"));
	Spire02->SetupAttachment(Platform);
	Spire02->SetWorldLocation(FVector(0.0f, 0.0f, 115.0f));
	Spire02->SetWorldRotation(FRotator(115.0f, 270.0f, 335.0f));
	Spire02->SetWorldScale3D(FVector(15.0f, 15.0f, 15.0f));
	Spire02->SetMobility(EComponentMobility::Static);

	SetReplicates(true);
}

void AC_PlayerBase::OnHealthChanged()
{
	for (UMaterialInstanceDynamic* MID : MID_Mats)
	{
		float NewHealthVal = Health / HealthMax;
		if (!MID) return;
		MID->SetScalarParameterValue(FName("Health"), NewHealthVal);
	}

	if (Health > 0) return;

	IsDead = true;

	Spire01->SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f));
	Spire02->SetWorldScale3D(FVector(0.0f, 0.0f, 0.0f));

	FTransform BaseTop = FTransform(FRotator(0.0f, 0.0f, 0.0f), Spire01->GetComponentLocation(), FVector(5.0f, 5.0f, 5.0f));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BaseEndParticle, BaseTop, true, EPSCPoolMethod::None, true);

	II_MenuConnect* MenuOwner{ Cast<II_MenuConnect>(GetOwner()) };
	if (!MenuOwner) return;
	MenuOwner->MakeEndScreen();

	ForHealthChange_SERVER();
}

void AC_PlayerBase::ForHealthChange_SERVER_Implementation()
{
	II_MenuConnect* MenuOwner{ Cast<II_MenuConnect>(GetOwner()) };
	if (!MenuOwner) return;
	MenuOwner->MakeEndScreen();

	GetWorldTimerManager().PauseTimer(RegenTimer);
}

void AC_PlayerBase::OnMoneyChanged()
{
	float MoneyDiff{ static_cast<float>(Money) / static_cast<float>(MoneyMaxForBase) };
	float MoneyLerp{ FMath::Lerp(MinSpireScale, MaxSpireScale, MoneyDiff) };
	float NewSpireScale{ FMath::Clamp(MoneyLerp, MinSpireScale, MaxSpireScale) };
	FVector NewScale{ NewSpireScale, NewSpireScale, NewSpireScale };

	Spire01->SetWorldScale3D(NewScale);
	Spire02->SetWorldScale3D(NewScale);
}

void AC_PlayerBase::RegenBaseHealth()
{
	if (Health >= HealthMax) return;
	ChangeHealthOfBase(HealthRegenScale);
}

void AC_PlayerBase::ClaimBase()
{
	if (!GetOwner()) return;
	ChangeMoneyOfBase(0);
	ChangeHealthOfBase(HealthMax);
}

void AC_PlayerBase::ChangeHealthOfBase(float AddHealth)
{
	if (IsDead) return;

	float NewHealth{ Health + AddHealth };
	NewHealth = FMath::Clamp(NewHealth, 0.0f, HealthMax);
	Health = NewHealth;

	OnHealthChanged();
}

void AC_PlayerBase::ChangeMoneyOfBase(int32 AddMoney)
{
	if (IsDead) return;

	int32 NewMoney{ Money + AddMoney };
	if (NewMoney <= 0) { Money = 0; }
	else { Money = NewMoney; }

	OnMoneyChanged();
}

void AC_PlayerBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(RegenTimer, [this]() { RegenBaseHealth(); }, 1.0f, true);
	Spire01->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	Spire02->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	TArray<UStaticMeshComponent*> CompsToProcess{ Platform, Spire01, Spire02 };

	MID_Mats.SetNum(CompsToProcess.Num());

	for (int32 i = 0; i < CompsToProcess.Num(); ++i)
	{
		UStaticMeshComponent* Comp{ CompsToProcess[i] };
		MID_Mats[i] = Comp->CreateDynamicMaterialInstance(0, Comp->GetMaterial(0));
		Comp->SetMaterial(0, MID_Mats[i]);
	}
}

void AC_PlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FString Mess = "H (" + FString::SanitizeFloat(Health) + "), M (" + FString::FromInt(Money) + ")";
	UKismetSystemLibrary::PrintString(GetWorld(), Mess, true, false, FLinearColor(FColor::Emerald), 0.0f);
}

void AC_PlayerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AC_PlayerBase, Health);
	DOREPLIFETIME(AC_PlayerBase, Money);
}

///////////////////FOR_BP_TESTING_ONLY
bool AC_PlayerBase::CheckForBaseBP_Implementation() { return true; }
void AC_PlayerBase::ChangeHealthOfBaseBP_Implementation(float AddHealth) {}
void AC_PlayerBase::ChangeMoneyOfBaseBP_Implementation(int32 AddMoney) {}
void AC_PlayerBase::ClaimBaseBP_Implementation() {}
