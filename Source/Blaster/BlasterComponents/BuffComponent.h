// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UBuffComponent();
	friend class ABlasterCharacter;
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffBaseCouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);

	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float BaseJumpVelocity);

protected:
	
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRamUp(float DeltaTime);

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	class ABlasterCharacter* Character;

	/*
	* Heal Buff
	*/
	bool bHealing = false;
	float HealingRate = 0.0f;
	float AmountToHeal = 0.0f;

	/*
	* Speed Buff
	*/
	FTimerHandle SpeedBuffTimer;
	void ResetSpeed();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	/*
	*  Jump Buff
	*/
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;

	/*
	* Shield Buff
	*/
	bool bReplenishingShield = false;
	float ReplenishingRate = 0.0f;
	float ShieldReplenishAmount = 0.0f;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float BaseJumpvelocity);
};
