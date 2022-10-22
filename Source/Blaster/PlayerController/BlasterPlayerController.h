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
class ABlasterGameMode;

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
	void SetHUDAnnouncementCountDown(float CountdownTime);

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

	void HandleMatchHasStarted();
	void HandleCoolDown();

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

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();


	//Client RPC to avoid replicating Warmuptime, MatchTime and StartingTime
	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float CoolDown);

private:
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	float LevelStartingTime = 0.0f;
	float WarmUpTime = 0.0f;
	float MatchTime = 0.0f;
	float CoolDownTime = 0.0f;
	uint32 CountdownInt = 0;

	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

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
