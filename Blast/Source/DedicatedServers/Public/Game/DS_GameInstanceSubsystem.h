// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DS_GameInstanceSubsystem.generated.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#endif

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UDS_GameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UDS_GameInstanceSubsystem();

#if WITH_GAMELIFT
	void InitGameLift(const FServerParameters& ServerParams);
#endif
	UPROPERTY(BlueprintReadOnly)
	bool bGameLiftInitialized;

private:
	void ParseCommandLinePort(int32& OutPort);

#if WITH_GAMELIFT
	FProcessParameters ProcessParameters;
#endif
};
