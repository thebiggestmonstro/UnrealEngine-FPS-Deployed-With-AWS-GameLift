// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/PortalManagement.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "PortalManager.generated.h"


/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UPortalManager : public UHTTPRequestManager, public IPortalManagement
{
	GENERATED_BODY()

public:
	void SignIn(const FString& Username, const FString& Password);
	void SignUp(const FString& Username, const FString& Password, const FString& Email);
	// IPortalManagement Interfaces
	virtual void RefreshTokens(const FString& RefreshToken) override;
	void Confirm(const FString& ConfirmationCode);

	UFUNCTION()
	void QuitGame();

	FDSSignUpResponse LastSignUpResponse;
	FString LastUsername;

	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage SignUpStatusMessageDelegate;

	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage ConfirmStatusMessageDelegate;

	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage SignInStatusMessageDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnSignUpSucceeded;

	UPROPERTY(BlueprintAssignable)
	FOnAPIRequestSucceeded OnConfirmSucceeded;

private:
	void SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void Confirm_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
