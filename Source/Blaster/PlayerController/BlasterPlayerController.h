// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */

class ABlasterHUD;
class UCharacterOverlay;

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDeath(int32 DeathsAmount);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDWeaponMagAmmo(int32 MagSize);
	void SetHUDMatchCountDown(float CountdownTime);
	void OnMatchState(FName State);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ReceivedPlayer() override;
	virtual float GetServerTime();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	/*Sync Time between Client and Server*/


	//request the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeofServerRecieveClientRequest);

	//Differences between Client and Server Time
	float ClientServerDelta = 0.0f;

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.0f;

	float TimeSyncRunningTime = 0.0f;

	void CheckTimeSync(float DeltaTime);

private:
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	/*To Remove because matchTime should be held by GameMode Class*/
	float MatchTime = 120.0f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	
	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDeaths;
};
