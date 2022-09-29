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

void ABlasterPlayerController::SetHUDScore(float Score)
{
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharcterOverlay &&
		BlasterHUD->CharcterOverlay->ScoreAmount;

	if(bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), (int)Score);
		BlasterHUD->CharcterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}
