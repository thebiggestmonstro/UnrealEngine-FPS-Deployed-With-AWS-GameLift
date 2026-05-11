// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DS_GameMode.generated.h"

struct FServerParameters;
struct FProcessParameters;

DECLARE_LOG_CATEGORY_EXTERN(LogDS_GameMode, Log, All);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ADS_GameMode();

protected:
	virtual void BeginPlay() override;

private:
	void SetServerParameters(FServerParameters& OutServerParameters);
	void InitGameLift();

private:
	TSharedPtr<FProcessParameters> ProcessParameters;
};
