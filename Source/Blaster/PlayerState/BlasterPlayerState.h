// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */

class ABlasterCharacter;
class ABlasterPlayerController;

UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/*
	//Replication Notifies
	*/
	virtual void OnRep_Score() override;
	
	UFUNCTION()
	virtual void OnRep_Death();

	void AddToScore(float ScoreAmount);
	void AddToDeath(int32 DeathsAmount);

private:
	UPROPERTY()
	ABlasterCharacter* Character ;

	UPROPERTY()
	ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Death)
	int32 Deaths;
};
