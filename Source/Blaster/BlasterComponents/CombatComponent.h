// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterType/CombatState.h"
#include "CombatComponent.generated.h"

#define  TRACE_LENGTH 80000.f
class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterHUD;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend ABlasterCharacter;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	void FireButtonPressed(bool bPressed);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	void Fire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	void AutoReload();
	int32 AmountToReload();
	void PlayEquipSound();
	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);
	void LoadCrosshairTexture(float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerController* Controller;

	UPROPERTY()
	ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	//HUD and Crosshairs
	float BaseCrosshairSpread = 0.50f;
	float CrosshairVelocityFactor;
	float CrosshairJumpFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	float CrosshairPlayerFactor;
	bool PlayerDeteched;

	/*Aiming and FOV
	*/

	//Field of View when not Aiming
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.0f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.0f;

	void InterpFOV(float DeltaTime);
		
	/*
	//Automatic Fire
	*/
	FTimerHandle FireTimer;

	void StartFireTimer();
	void FireTimerFinish();

	bool bCanFire = true;

	bool CanFire();

	//Carried Ammo for the currently Equipped Weapon
	UPROPERTY(ReplicatedUsing = OnRep_WeaponMagAmmo)
	int32 WeaponMagAmmo;

	UFUNCTION()
	void OnRep_WeaponMagAmmo();

	TMap<EWeaponType, int32> WeaponMagAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartingARMag = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	void InitializeMagAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_UnOccupied;

	UFUNCTION()
	void OnRep_CombatState();
};
