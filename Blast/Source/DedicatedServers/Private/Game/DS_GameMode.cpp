// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/Game/DS_GameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

#include "GenericPlatform/GenericPlatformOutputDevices.h"

DEFINE_LOG_CATEGORY(LogDS_GameMode);

ADS_GameMode::ADS_GameMode() : ProcessParameters(nullptr)
{
    UE_LOG(LogDS_GameMode, Log, TEXT("Initializing ADS_GameMode..."));
}

void ADS_GameMode::BeginPlay()
{
    Super::BeginPlay();

#if WITH_GAMELIFT
    InitGameLift();
#endif
}

#if WITH_GAMELIFT
void ADS_GameMode::SetServerParameters(FServerParameters& OutServerParameters)
{
    UE_LOG(LogDS_GameMode, Log, TEXT("Configuring server parameters for Anywhere..."));

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

    UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_YELLOW);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> WebSocket URL: %s"), *OutServerParameters.m_webSocketUrl);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Fleet ID: %s"), *OutServerParameters.m_fleetId);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Process ID: %s"), *OutServerParameters.m_processId);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Host ID (Compute Name): %s"), *OutServerParameters.m_hostId);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Auth Token: %s"), *OutServerParameters.m_authToken);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Aws Region: %s"), *OutServerParameters.m_awsRegion);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Access Key: %s"), *OutServerParameters.m_accessKey);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Secret Key: %s"), *OutServerParameters.m_secretKey);
    UE_LOG(LogDS_GameMode, Log, TEXT(">>>> Session Token: %s"), *OutServerParameters.m_sessionToken);
    UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_NONE);
}
#endif

void ADS_GameMode::InitGameLift()
{
#if WITH_GAMELIFT
    UE_LOG(LogDS_GameMode, Log, TEXT("Calling InitGameLift..."));

    // Getting the module first.
    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

    //Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
    FServerParameters ServerParametersForAnywhere;

    bool bIsAnywhereActive = false;
    if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
    {
        bIsAnywhereActive = true;
    }

    if (bIsAnywhereActive)
    {
        SetServerParameters(ServerParametersForAnywhere);
    }

    UE_LOG(LogDS_GameMode, Log, TEXT("Initializing the GameLift Server..."));

    //InitSDK will establish a local connection with GameLift's agent to enable further communication.
    FGameLiftGenericOutcome InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);
    if (InitSdkOutcome.IsSuccess())
    {
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(LogDS_GameMode, Log, TEXT("GameLift InitSDK succeeded!"));
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(LogDS_GameMode, Log, TEXT("ERROR: InitSDK failed : ("));
        FGameLiftError GameLiftError = InitSdkOutcome.GetError();
        UE_LOG(LogDS_GameMode, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_NONE);
        return;
    }

    ProcessParameters = MakeShared<FProcessParameters>();

    ProcessParameters->OnStartGameSession.BindLambda([=](Aws::GameLift::Server::Model::GameSession InGameSession)
        {
            FString GameSessionId = FString(InGameSession.GetGameSessionId());
            UE_LOG(LogDS_GameMode, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
            GameLiftSdkModule->ActivateGameSession();
        });

    ProcessParameters->OnTerminate.BindLambda([=]()
        {
            UE_LOG(LogDS_GameMode, Log, TEXT("Game Server Process is terminating"));
            // First call ProcessEnding()
            FGameLiftGenericOutcome processEndingOutcome = GameLiftSdkModule->ProcessEnding();
            // Then call Destroy() to free the SDK from memory
            FGameLiftGenericOutcome destroyOutcome = GameLiftSdkModule->Destroy();
            // Exit the process with success or failure
            if (processEndingOutcome.IsSuccess() && destroyOutcome.IsSuccess()) {
                UE_LOG(LogDS_GameMode, Log, TEXT("Server process ending successfully"));
            }
            else {
                if (!processEndingOutcome.IsSuccess()) {
                    const FGameLiftError& error = processEndingOutcome.GetError();
                    UE_LOG(LogDS_GameMode, Error, TEXT("ProcessEnding() failed. Error: %s"),
                        error.m_errorMessage.IsEmpty() ? TEXT("Unknown error") : *error.m_errorMessage);
                }
                if (!destroyOutcome.IsSuccess()) {
                    const FGameLiftError& error = destroyOutcome.GetError();
                    UE_LOG(LogDS_GameMode, Error, TEXT("Destroy() failed. Error: %s"),
                        error.m_errorMessage.IsEmpty() ? TEXT("Unknown error") : *error.m_errorMessage);
                }
            }
        });

    ProcessParameters->OnHealthCheck.BindLambda([]()
        {
            UE_LOG(LogDS_GameMode, Log, TEXT("Performing Health Check"));
            return true;
        });

    int32 Port = FURL::UrlConfig.DefaultPort;
    FParse::Value(FCommandLine::Get(), TEXT("port="), Port);
    ProcessParameters->port = Port;

    TArray<FString> Logfiles;
    Logfiles.Add(TEXT("GameLiftUnrealApp/Saved/Logs/FPSTemplate.log"));
    ProcessParameters->logParameters = Logfiles;

    UE_LOG(LogDS_GameMode, Log, TEXT("Calling Process Ready..."));
    FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);

    if (ProcessReadyOutcome.IsSuccess())
    {
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(LogDS_GameMode, Log, TEXT("Process Ready!"));
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(LogDS_GameMode, Log, TEXT("ERROR: Process Ready Failed!"));
        FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
        UE_LOG(LogDS_GameMode, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
        UE_LOG(LogDS_GameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(LogDS_GameMode, Log, TEXT("InitGameLift completed!"));
#endif
}



