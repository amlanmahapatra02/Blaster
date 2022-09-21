// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletCasing.generated.h"

class UStaticMeshComponent;
class USoundCue;

UCLASS()
class BLASTER_API ABulletCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletCasing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalInput, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectImpluse;

	UPROPERTY(EditAnywhere)
	USoundCue* ShellSound;

	FTimerHandle ShellDestroyTimerHandle;

	void PlayShellSound();

};
