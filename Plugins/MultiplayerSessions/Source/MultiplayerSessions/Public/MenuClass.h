// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuClass.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenuClass : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnection = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPersonCPP/Maps/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel *InLevel, UWorld *InWorld) override;

private:
	// The name should be same in bluprint widget to work
	UPROPERTY(meta = (BindWidget))
	class UButton *HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton *JoinButton;

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnCreateSession(bool bwasSuccessful);

	void OnFindSession(const TArray<FOnlineSessionSearchResult> &SessionResult, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	void MenuTearDown();

	// The susbsystem designed to handle all online session functionality
	class UMultiplayerSessionSubsystem *MultiplayerSessionSubsystem;

	int32 NumPublicConnections = 4;
	FString MatchType = FString(TEXT("FreeForAll"));
	FString PathToLobby = FString(TEXT(""));
};
