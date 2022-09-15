// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (BlasterCharacter)
	{
		FVector Velocity = BlasterCharacter->GetVelocity();
		Velocity.Z = 0.0f;

		Speed = Velocity.Size();

		bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
		bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
		bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
		EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
		bIsCrouched = BlasterCharacter->bIsCrouched;
		bAiming = BlasterCharacter->IsAiming();
		TurningInPlace = BlasterCharacter->GetTurningInPlace();

		// Offset Yaw for Strafing (Replicated both on Server and Clients)
		FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, InterpSpeed);
		YawOffset = DeltaRotation.Yaw;

		// Lean (Replicated both on Server and Clients)
		CharacterRotationLastFrame = CharacterRotationCurrentFrame;
		CharacterRotationCurrentFrame = BlasterCharacter->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotationCurrentFrame, CharacterRotationLastFrame);
		float Target = Delta.Yaw / DeltaTime;
		float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, InterpSpeed);
		Lean = FMath::Clamp(Interp, -89.f, 89.f);

		AO_Yaw = BlasterCharacter->GetAO_Yaw();
		AO_Pitch = BlasterCharacter->GetAO_Pitch();

		if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			//No need to replicate the HitResult because it not imp to take all the bandwidth for a cosmetic effect
			if (BlasterCharacter->IsLocallyControlled())
			{
				bLocallyControlled = true;
				FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
				RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			}
		}
	}
}

//Draw Two Debug line from Muzzle Tip and HitTarget
void UBlasterAnimInstance::DrawTwoDebugLineForGunPoint()
{
	FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
	FVector MuzzleX = FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X);

	FVector LineStart = MuzzleTipTransform.GetLocation();
	FVector LineEnd = LineStart + MuzzleX * 1000.0f;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Red);
	DrawDebugLine(GetWorld(), LineStart, BlasterCharacter->GetHitTarget(), FColor::Orange);
}
