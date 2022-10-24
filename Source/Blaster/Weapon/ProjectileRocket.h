// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileRocket();

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalInput, const FHitResult& Hit) override;
	
private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	UPROPERTY(EditDefaultsOnly)
	float MinimumDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	float InnerRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	float OuterRadius = 500.0f;

	float FallOff = 1.0f;
};
