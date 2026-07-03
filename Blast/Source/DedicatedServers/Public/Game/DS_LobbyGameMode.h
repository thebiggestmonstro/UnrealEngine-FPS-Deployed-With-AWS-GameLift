// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "DS_GameModeBase.h"
#include "DS_LobbyGameMode.generated.h"

class UDS_GameInstanceSubsystem;

DECLARE_LOG_CATEGORY_EXTERN(LogDS_LobbyGameMode, Log, All);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_LobbyGameMode : public ADS_GameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	void SetServerParameters(FServerParameters& OutServerParameters);
	void InitGameLift();

private:
	UPROPERTY()
	TObjectPtr<UDS_GameInstanceSubsystem> DSGameInstanceSubsystem;
};
