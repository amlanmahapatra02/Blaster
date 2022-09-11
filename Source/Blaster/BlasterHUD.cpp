// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewPortSize;

	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
		const FVector2D ViewPortCenter = {ViewPortSize.X/2.0f, ViewPortSize.Y/2.0f};

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread = { 0.0f, 0.0f };
			DrawCrosshairs(HUDPackage.CrosshairsCenter, ViewPortCenter, Spread);
		}

		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread = {-SpreadScaled , 0.0f };
			DrawCrosshairs(HUDPackage.CrosshairsLeft, ViewPortCenter, Spread);
		}

		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread = {SpreadScaled , 0.0f };
			DrawCrosshairs(HUDPackage.CrosshairsRight, ViewPortCenter,Spread);
		}

		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread = { 0.0f, -SpreadScaled };
			DrawCrosshairs(HUDPackage.CrosshairsTop, ViewPortCenter,Spread);
		}

		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread = { 0.0f, SpreadScaled };
			DrawCrosshairs(HUDPackage.CrosshairsBottom, ViewPortCenter,Spread);
		}
	}
}

void ABlasterHUD::DrawCrosshairs(UTexture2D* Texture, FVector2D ViewPortCenter, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();

	FVector2D TextureDrawPoint = { ViewPortCenter.X - (TextureWidth / 2.0f) + Spread.X,
								   ViewPortCenter.Y - (TextureHeight / 2.0f) + Spread.Y};

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		FLinearColor::Red
	);
}
