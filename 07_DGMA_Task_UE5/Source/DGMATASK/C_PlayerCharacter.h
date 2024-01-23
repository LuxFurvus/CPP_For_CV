// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Particles/ParticleSystem.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundBase.h"
#include "Engine/EngineTypes.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
//////////////////////////////////////
#include "F_TurretTraits.h"
#include "I_MenuConnect.h"
#include "C_Turret.h"
//////////////////////////////////////
#include "C_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AC_PlayerCharacter : public ACharacter, public II_MenuConnect
{
	GENERATED_BODY()

	//////////////////////////////////////////////
	/////////////+++COMMON MEMBERS+++///////////////////////
	/////////////////////////////////////////////////////////////////////

public:	AC_PlayerCharacter();
protected: void BeginPlay() override;
public:	void Tick(float DeltaTime) override;

public: FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
public: FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private: UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom{ nullptr };
private: UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera{ nullptr };

protected: UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* SphereBody{ nullptr };

///////////// INPUT VARIABLES

private: UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = INPUT, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext{ nullptr };
private: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = INPUT, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction{ nullptr };
private: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = INPUT, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction{ nullptr };

//////////////// INPUT FUNCTIONS

protected: void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected: void Move(const FInputActionValue& Value);
protected: void Look(const FInputActionValue& Value);


//////////////////////////////////////////////
/////////////+++TASK 1+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////

private: float BuildDistance{ 10000.0f };
private: bool IsTurretSelected{ false };
private: FName TurrColor;

protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	bool HasPlaceToBuild{ false };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	FVector PlaceToBuild;
protected: UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	bool CanBuildTurret;

protected: UPROPERTY(Replicated, BlueprintReadOnly, Category = TO_SET) F_TurretTraits TurretTraits;

protected: UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TO_SET)
		 UMaterialInstanceDynamic* BuildColor{ nullptr };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TO_SET)
		 UUserWidget* WBP_BuildMenu{ nullptr };
protected: UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		 UStaticMeshComponent* TurretProjection{ nullptr };
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	FDataTableRowHandle DT_TurretTraits;
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	TSubclassOf<UUserWidget> BuildScreenClass;
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	TSubclassOf<AC_Turret> TurretClass;

protected: UFUNCTION(BlueprintCallable) 
	void SelectTurretColor(const FName& Color);
	
private: void LookForBuildPlace();

//////////////////////////////////////////////
/////////////+++TASK 2+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////////////+++2.b (Building Mode)

private: bool IsInBuildingMode{ false };
private: UMaterialInterface* NonBuildMat{ nullptr };

protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	UMaterialInterface* BuildMat{ nullptr };

protected: UFUNCTION(BlueprintCallable) 
		 void OpenBuildMode();
private: UFUNCTION(Server, Reliable) 
		 void OpenBuildMode_SERVER();
private: UFUNCTION(NetMulticast, Reliable) 
		 void OpenBuildMode_MULTI();

/////////////+++2.c (Building Cooldown)

private: FTimerHandle BuildCooldownTimer;
private: TObjectPtr<UStaticMesh> NonBuildShape;

protected: float LastBuildTime{ 0.0f };
protected: bool CanBuild{ true };

protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	TObjectPtr<UStaticMesh> BuildShape;
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TO_SET)
	float BuildCooldownTime{ 10.0f };

protected: void CheckBuildAbility();

protected: UFUNCTION(BlueprintCallable)
	void BuildTurret(const FDataTableRowHandle& Traits);
protected: UFUNCTION(BlueprintCallable, Server, Reliable)
	void BuildTurret_SERVER(const FDataTableRowHandle& Traits);
protected: UFUNCTION(NetMulticast, Reliable)
	void BuildTurret_MULTI();
protected: UFUNCTION(BlueprintCallable, Server, Reliable)
	void KillTurret_SERVER(int32 TurretPrice);

private: UFUNCTION(Server, Reliable) void SetUnbuildable_SERVER(bool CanBuildNow);
private: UFUNCTION(NetMulticast, Reliable) void SetUnbuildable_MULTI(bool CanBuildNow);


//////////////////////////////////////////////
/////////////+++TASK 4+++///////////////////////////////////
/////////////////////////////////////////////////////////////////////

private: bool IsDead{ false };

protected: AActor* PlayerBase{ nullptr };
protected: UUserWidget* PlayerScreen{ nullptr };

protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	TSubclassOf<UUserWidget> PlayerScreenClass;
protected: UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = TO_SET)
	USoundBase* ClaimBaseSound{ nullptr };

protected: UFUNCTION(BlueprintCallable, Client, Reliable)
	void SetPlayerBaseLink_CLIENT();
private: UFUNCTION(Server, Reliable)
	void SetPlayerBaseLink_SERVER(AActor* PlayerBaseActor);
private: UFUNCTION(NetMulticast, Reliable)
	void SetPlayerBaseLink_MULTI(AActor* PlayerBaseActor);

protected: UFUNCTION(BlueprintCallable, Server, Reliable)
	void TEST_ChangeBaseHealth_SERVER(float AddHealth);

private: void MakeEndScreen() override;
protected: UFUNCTION(Server, Reliable)
	void MakeEndScreen_SERVER();
protected: UFUNCTION(NetMulticast, Reliable)
	void MakeEndScreen_MULTI();

protected: UFUNCTION(Server, Reliable)
	void MakeMat_SERVER();
protected: UFUNCTION(NetMulticast, Reliable)
	void MakeMat_MULTI();

///////////FOR_BP_TESTING_ONLY
protected: void MakeEndScreenBP_Implementation() override;
};

