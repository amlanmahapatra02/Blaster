// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
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

void ABlasterPlayerController::SetHUDWeaponMagAmmo(int32 MagSize)
{
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponMagAmount;

	if (bHUDValid)
	{
		FString WeaponMagText = FString::Printf(TEXT("%d"), MagSize);
		BlasterHUD->CharacterOverlay->WeaponMagAmount->SetText(FText::FromString(WeaponMagText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}

void ABlasterPlayerController::SetHUDMatchCountDown(float CountdownTime)
{
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay && 
		BlasterHUD->CharacterOverlay->MatchCountDownText;

	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60.0f;

		FString CountDownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountDownText));
	}
	else
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	uint32 SecondLefts = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondLefts)
	{
		SetHUDMatchCountDown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondLefts;
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeofServerRecieveClientRequest)
{
	float ClientTime = GetWorld()->GetTimeSeconds();
	float RoundTripTime = ClientTime - TimeOfClientRequest;
	float CurrentServerTime = TimeofServerRecieveClientRequest - (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - ClientTime;
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
} 

float ABlasterPlayerController::GetServerTime()
{
	float ServerTime = GetWorld()->GetTimeSeconds();
	if (HasAuthority())
	{
		return ServerTime;
	}
	else
	{
		return ServerTime + ClientServerDelta;
	}
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

