// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/BlasterHUD.h"
#include "Blaster/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	bool bHUDValid = BlasterHUD &&
					 BlasterHUD->CharcterOverlay &&
					 BlasterHUD->CharcterOverlay->HealthBar &&
					 BlasterHUD->CharcterOverlay->HealthText;

	if (bHUDValid)
	{
		//Health Bar
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharcterOverlay->HealthBar->SetPercent(HealthPercent);

		//Health Text
		FString HealthText = FString::Printf(TEXT("%d/%d"), (int)Health, (int)MaxHealth);
		BlasterHUD->CharcterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}
