// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class BLASTER_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayerUser;

	void SetDisplayText(FString TextToDisplay, FString DisplayUserText);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);


protected:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
};
