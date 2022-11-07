// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.0f;
	AimWalkSpeed = 450.0f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, WeaponMagAmmo, COND_OwnerOnly);
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}

		if (Character->HasAuthority())
		{
			InitializeMagAmmo();
		}
	}
	
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon)
	{
		if(bAiming)
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
		}
		else
		{
			CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
		}

		if(Character && Character->GetFollowCamera())
		{
			Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
		}
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon && Character)
	{
		Character->GetWorldTimerManager().SetTimer(
			FireTimer,
			this,
			&UCombatComponent::FireTimerFinish,
			EquippedWeapon->FireDelay
		);
	}
}

void UCombatComponent::FireTimerFinish()
{
	if (EquippedWeapon)
	{
		bCanFire = true;
		if (bFireButtonPressed && EquippedWeapon->bAutomatic)
		{
			Fire();
		}
		//if Empty Auto Reload
		AutoReload();
	}
}


void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character && WeaponToEquip)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->DropWeapon();
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}

		EquippedWeapon->SetOwner(Character);
		EquippedWeapon->SetHUDAmmo();

		if (WeaponMagAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		{
			WeaponMagAmmo = WeaponMagAmmoMap[EquippedWeapon->GetWeaponType()];
		}
		
		//for server player only
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDWeaponMagAmmo(WeaponMagAmmo);
		}

		PlayEquipSound();

		//Server only player
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		AutoReload();
	}
}

void UCombatComponent::PlayEquipSound()
{
	if (EquippedWeapon && EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
	}
}

void UCombatComponent::AutoReload()
{
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
	UE_LOG(LogTemp, Warning, TEXT("Reloading Working"));
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon)
	{
		int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

		if (WeaponMagAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		{
			int32 AmmountCarried = WeaponMagAmmoMap[EquippedWeapon->GetWeaponType()];
			int32 least = FMath::Min(RoomInMag, AmmountCarried);
			return FMath::Clamp(RoomInMag, 0, least);
		}
	}
	return 0;
}

void UCombatComponent::Reload()
{
	if (WeaponMagAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character && EquippedWeapon)
	{
		int32 ReloadAmount = AmountToReload();
		if (WeaponMagAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		{
			WeaponMagAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
			WeaponMagAmmo = WeaponMagAmmoMap[EquippedWeapon->GetWeaponType()];
		}

		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDWeaponMagAmmo(WeaponMagAmmo);
		}
		
		if (EquippedWeapon->GetAmmo() < EquippedWeapon->GetMagCapacity())
		{
			CombatState = ECombatState::ECS_Reloading;
			HandleReload();
			EquippedWeapon->AddAmmo(-ReloadAmount);
		}
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character)
	{
		if (Character->HasAuthority())
		{
			CombatState = ECombatState::ECS_UnOccupied;
		}
		
		if (bFireButtonPressed)
		{
			Fire();
		}
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{;
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;

	case ECombatState::ECS_UnOccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	//Client side replication of strafe
	if (Character && EquippedWeapon)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		PlayEquipSound();
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed && EquippedWeapon)
	{
		Fire();
	}
}
	
void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		ServerFire(HitTarget);
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 0.65f;
		}
		StartFireTimer();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character && EquippedWeapon && CombatState == ECombatState::ECS_UnOccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
						 (
							UGameplayStatics::GetPlayerController(this, 0),
							CrosshairLocation,
							CrosshairWorldPosition,
							CrosshairWorldDirection
						);

	if (bScreenToWorld)
	{
		//Line Trace Start and End points
		FVector Start = CrosshairWorldPosition;

		if(Character)
		{
			float DistancesToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistancesToCharacter + 100.0f);
		}

		FVector End = Start + (CrosshairWorldDirection * TRACE_LENGTH);

		//Performing LineTrace
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		//Checking if the line trace hits an character to change the crosshair hud to red
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
			PlayerDeteched = true;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
			PlayerDeteched = false;
		}
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character && Character->Controller)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;

			if (HUD)
			{
				LoadCrosshairTexture(DeltaTime);
			}
		}
	}
}

void UCombatComponent::LoadCrosshairTexture(float DeltaTime)
{

	if (EquippedWeapon)
	{
		HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCentre;
		HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
		HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
		HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
		HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
	}

	else
	{
		HUDPackage.CrosshairsCenter = nullptr;
		HUDPackage.CrosshairsLeft = nullptr;
		HUDPackage.CrosshairsRight = nullptr;
		HUDPackage.CrosshairsTop = nullptr;
		HUDPackage.CrosshairsBottom = nullptr;
	}

	//Calculate Crosshair Spread

	//Velocity Factor
	FVector2D WalkSpeedRange = { 0.0f, Character->GetCharacterMovement()->MaxWalkSpeed };
	FVector2D VelocityMultiplerRange = { 0.0f, 1.0f };
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.0f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplerRange, Velocity.Size());

	//Jump Factor
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairJumpFactor = FMath::FInterpTo(CrosshairJumpFactor, 2.5f, DeltaTime, 7.0f);
	}
	else
	{
		CrosshairJumpFactor = FMath::FInterpTo(CrosshairJumpFactor, 0.0f, DeltaTime, 35.0f);
	}

	//Aim Factor
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.55f, DeltaTime, 30.0f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.0f);
	}

	if (PlayerDeteched)
	{
		CrosshairPlayerFactor = FMath::FInterpTo(CrosshairPlayerFactor, 0.3f, DeltaTime, 30.0f);
	}
	else
	{
		CrosshairPlayerFactor = FMath::FInterpTo(CrosshairPlayerFactor, 0.0f, DeltaTime, 30.0f);
	}

	CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 40.0f);

	HUDPackage.CrosshairSpread = BaseCrosshairSpread + CrosshairVelocityFactor + CrosshairJumpFactor - CrosshairAimFactor + CrosshairShootingFactor - CrosshairPlayerFactor;
	HUD->SetHUDPackage(HUDPackage);
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon)
	{
		return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_UnOccupied;
	}
	else
	{
		return false;
	}
}

void UCombatComponent::OnRep_WeaponMagAmmo()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDWeaponMagAmmo(WeaponMagAmmo);
	}
}

void UCombatComponent::InitializeMagAmmo()
{
	WeaponMagAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARMag);
	WeaponMagAmmoMap.Emplace(EWeaponType::EWT_RocketLaucher, StartingRocketAmmo);
	WeaponMagAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	WeaponMagAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSMGAmmo);
	WeaponMagAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
}