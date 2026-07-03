// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/Game/DS_LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Game/DS_GameInstanceSubsystem.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

#include "GenericPlatform/GenericPlatformOutputDevices.h"

DEFINE_LOG_CATEGORY(LogDS_LobbyGameMode);

void ADS_LobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

#if WITH_GAMELIFT
    InitGameLift();
#endif
}

#if WITH_GAMELIFT
void ADS_LobbyGameMode::SetServerParameters(FServerParameters& OutServerParameters)
{
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT("Configuring server parameters for Anywhere..."));

    // If GameLift Anywhere is enabled, parse command line arguments and pass them in the ServerParameters object.
    FString glAnywhereWebSocketUrl = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereWebSocketUrl="), glAnywhereWebSocketUrl))
    {
        OutServerParameters.m_webSocketUrl = TCHAR_TO_UTF8(*glAnywhereWebSocketUrl);
    }

    FString glAnywhereFleetId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereFleetId="), glAnywhereFleetId))
    {
        OutServerParameters.m_fleetId = TCHAR_TO_UTF8(*glAnywhereFleetId);
    }

    FString glAnywhereProcessId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereProcessId="), glAnywhereProcessId))
    {
        OutServerParameters.m_processId = TCHAR_TO_UTF8(*glAnywhereProcessId);
    }
    else
    {
        // If no ProcessId is passed as a command line argument, generate a randomized unique string.
        FString TimeString = FString::FromInt(std::time(nullptr));
        FString ProcessId = "ProcessId_" + TimeString;
        OutServerParameters.m_processId = TCHAR_TO_UTF8(*ProcessId);
    }

    FString glAnywhereHostId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereHostId="), glAnywhereHostId))
    {
        OutServerParameters.m_hostId = TCHAR_TO_UTF8(*glAnywhereHostId);
    }

    FString glAnywhereAuthToken = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAuthToken="), glAnywhereAuthToken))
    {
        OutServerParameters.m_authToken = TCHAR_TO_UTF8(*glAnywhereAuthToken);
    }

    FString glAnywhereAwsRegion = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAwsRegion="), glAnywhereAwsRegion))
    {
        OutServerParameters.m_awsRegion = TCHAR_TO_UTF8(*glAnywhereAwsRegion);
    }

    FString glAnywhereAccessKey = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAccessKey="), glAnywhereAccessKey))
    {
        OutServerParameters.m_accessKey = TCHAR_TO_UTF8(*glAnywhereAccessKey);
    }

    FString glAnywhereSecretKey = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereSecretKey="), glAnywhereSecretKey))
    {
        OutServerParameters.m_secretKey = TCHAR_TO_UTF8(*glAnywhereSecretKey);
    }

    FString glAnywhereSessionToken = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereSessionToken="), glAnywhereSessionToken))
    {
        OutServerParameters.m_sessionToken = TCHAR_TO_UTF8(*glAnywhereSessionToken);
    }

    UE_LOG(LogDS_LobbyGameMode, SetColor, TEXT("%s"), COLOR_YELLOW);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> WebSocket URL: %s"), *OutServerParameters.m_webSocketUrl);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Fleet ID: %s"), *OutServerParameters.m_fleetId);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Process ID: %s"), *OutServerParameters.m_processId);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Host ID (Compute Name): %s"), *OutServerParameters.m_hostId);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Auth Token: %s"), *OutServerParameters.m_authToken);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Aws Region: %s"), *OutServerParameters.m_awsRegion);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Access Key: %s"), *OutServerParameters.m_accessKey);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Secret Key: %s"), *OutServerParameters.m_secretKey);
    UE_LOG(LogDS_LobbyGameMode, Log, TEXT(">>>> Session Token: %s"), *OutServerParameters.m_sessionToken);
    UE_LOG(LogDS_LobbyGameMode, SetColor, TEXT("%s"), COLOR_NONE);
}
#endif

void ADS_LobbyGameMode::InitGameLift()
{
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (DSGameInstanceSubsystem = GameInstance->GetSubsystem<UDS_GameInstanceSubsystem>(); IsValid(DSGameInstanceSubsystem))
        {
            FServerParameters ServerParameters;

            if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
            {
#if WITH_GAMELIFT
                SetServerParameters(ServerParameters);
#endif
            }

            DSGameInstanceSubsystem->InitGameLift(ServerParameters);
        }
    }
}