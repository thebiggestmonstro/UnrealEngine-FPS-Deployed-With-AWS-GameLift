// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "GameStatsManager.generated.h"

struct FDSRecordMatchStatsInput;
struct FDSLeaderboardItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRetrieveMatchStatsResponseReceived, const FDSRetrieveMatchStatsResponse&, RetrieveMatchStatsResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRetrieveLeaderboard, TArray<FDSLeaderboardItem>&, Leaderboard);

/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGameStatsManager : public UHTTPRequestManager
{
	GENERATED_BODY()
	
public:
	void RecordMatchStats(const FDSRecordMatchStatsInput& RecordMatchStatsInput);
	void RetrieveMatchStats();
	void UpdateLeaderboard(const TArray<FString>& WinnerUsernames);
	void RetrieveLeaderboard();

	UPROPERTY()
	FOnRetrieveMatchStatsResponseReceived OnRetrieveMatchStatsResponseReceived;

	UPROPERTY()
	FAPIStatusMessage RetrieveMatchStatsStatusMesssage;

	UPROPERTY()
	FAPIStatusMessage RetrieveLeaderboardStatusMessage;

	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnUpdateLeaderboardSucceeded;

	UPROPERTY()
	FOnRetrieveLeaderboard OnRetrieveLeaderboard;
	
private:
	void RecordMatchStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RetrieveMatchStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void UpdateLeaderboard_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RetrieveLeaderboard_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
