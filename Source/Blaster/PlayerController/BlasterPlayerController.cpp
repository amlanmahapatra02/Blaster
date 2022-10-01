// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	bool bHUDValid = BlasterHUD &&
					 BlasterHUD->CharacterOverlay &&
					 BlasterHUD->CharacterOverlay->HealthBar &&
					 BlasterHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		//Health Bar
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		//Health Text
		FString HealthText = FString::Printf(TEXT("%d/%d"), (int)Health, (int)MaxHealth);
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;

	if(bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), (int)Score);
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}

void ABlasterPlayerController::SetHUDDeath(int32 DeathsAmount)
{
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DeathsAmount;

	if (bHUDValid)
	{
		FString DeathText = FString::Printf(TEXT("%d"), DeathsAmount);
		BlasterHUD->CharacterOverlay->DeathsAmount->SetText(FText::FromString(DeathText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}
