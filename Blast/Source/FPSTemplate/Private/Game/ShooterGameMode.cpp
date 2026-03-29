// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ShooterGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

#include "GenericPlatform/GenericPlatformOutputDevices.h"

DEFINE_LOG_CATEGORY(LogShooterGameMode);

AShooterGameMode::AShooterGameMode() : ProcessParameters(nullptr)
{
    UE_LOG(LogShooterGameMode, Log, TEXT("Initializing AShooterGameMode..."));
}

void AShooterGameMode::BeginPlay()
{
    Super::BeginPlay();

#if WITH_GAMELIFT
    InitGameLift();
#endif
}

void AShooterGameMode::InitGameLift()
{
#if WITH_GAMELIFT
    UE_LOG(LogShooterGameMode, Log, TEXT("Calling InitGameLift..."));

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
        UE_LOG(LogShooterGameMode, Log, TEXT("Configuring server parameters for Anywhere..."));

        // If GameLift Anywhere is enabled, parse command line arguments and pass them in the ServerParameters object.
        FString glAnywhereWebSocketUrl = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereWebSocketUrl="), glAnywhereWebSocketUrl))
        {
            ServerParametersForAnywhere.m_webSocketUrl = TCHAR_TO_UTF8(*glAnywhereWebSocketUrl);
        }

        FString glAnywhereFleetId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereFleetId="), glAnywhereFleetId))
        {
            ServerParametersForAnywhere.m_fleetId = TCHAR_TO_UTF8(*glAnywhereFleetId);
        }

        FString glAnywhereProcessId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereProcessId="), glAnywhereProcessId))
        {
            ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*glAnywhereProcessId);
        }
        else
        {
            // If no ProcessId is passed as a command line argument, generate a randomized unique string.
            FString TimeString = FString::FromInt(std::time(nullptr));
            FString ProcessId = "ProcessId_" + TimeString;
            ServerParametersForAnywhere.m_processId = TCHAR_TO_UTF8(*ProcessId);
        }

        FString glAnywhereHostId = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereHostId="), glAnywhereHostId))
        {
            ServerParametersForAnywhere.m_hostId = TCHAR_TO_UTF8(*glAnywhereHostId);
        }

        FString glAnywhereAuthToken = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAuthToken="), glAnywhereAuthToken))
        {
            ServerParametersForAnywhere.m_authToken = TCHAR_TO_UTF8(*glAnywhereAuthToken);
        }

        FString glAnywhereAwsRegion = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAwsRegion="), glAnywhereAwsRegion))
        {
            ServerParametersForAnywhere.m_awsRegion = TCHAR_TO_UTF8(*glAnywhereAwsRegion);
        }

        FString glAnywhereAccessKey = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereAccessKey="), glAnywhereAccessKey))
        {
            ServerParametersForAnywhere.m_accessKey = TCHAR_TO_UTF8(*glAnywhereAccessKey);
        }

        FString glAnywhereSecretKey = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereSecretKey="), glAnywhereSecretKey))
        {
            ServerParametersForAnywhere.m_secretKey = TCHAR_TO_UTF8(*glAnywhereSecretKey);
        }

        FString glAnywhereSessionToken = "";
        if (FParse::Value(FCommandLine::Get(), TEXT("glAnywhereSessionToken="), glAnywhereSessionToken))
        {
            ServerParametersForAnywhere.m_sessionToken = TCHAR_TO_UTF8(*glAnywhereSessionToken);
        }

        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_YELLOW);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> WebSocket URL: %s"), *ServerParametersForAnywhere.m_webSocketUrl);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Fleet ID: %s"), *ServerParametersForAnywhere.m_fleetId);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Process ID: %s"), *ServerParametersForAnywhere.m_processId);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Host ID (Compute Name): %s"), *ServerParametersForAnywhere.m_hostId);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Auth Token: %s"), *ServerParametersForAnywhere.m_authToken);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Aws Region: %s"), *ServerParametersForAnywhere.m_awsRegion);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Access Key: %s"), *ServerParametersForAnywhere.m_accessKey);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Secret Key: %s"), *ServerParametersForAnywhere.m_secretKey);
        UE_LOG(LogShooterGameMode, Log, TEXT(">>>> Session Token: %s"), *ServerParametersForAnywhere.m_sessionToken);
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(LogShooterGameMode, Log, TEXT("Initializing the GameLift Server..."));

    //InitSDK will establish a local connection with GameLift's agent to enable further communication.
    FGameLiftGenericOutcome InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);
    if (InitSdkOutcome.IsSuccess())
    {
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(LogShooterGameMode, Log, TEXT("GameLift InitSDK succeeded!"));
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(LogShooterGameMode, Log, TEXT("ERROR: InitSDK failed : ("));
        FGameLiftError GameLiftError = InitSdkOutcome.GetError();
        UE_LOG(LogShooterGameMode, Log, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_NONE);
        return;
    }

    ProcessParameters = MakeShared<FProcessParameters>();

    //When a game session is created, Amazon GameLift Servers sends an activation request to the game server and passes along the game session object containing game properties and other settings.
    //Here is where a game server should take action based on the game session object.
    //Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
    ProcessParameters->OnStartGameSession.BindLambda([=](Aws::GameLift::Server::Model::GameSession InGameSession)
        {
            FString GameSessionId = FString(InGameSession.GetGameSessionId());
            UE_LOG(LogShooterGameMode, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
            GameLiftSdkModule->ActivateGameSession();
        });

    //OnProcessTerminate callback. Amazon GameLift Servers will invoke this callback before shutting down an instance hosting this game server.
    //It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
    //In this case, we simply tell Amazon GameLift Servers we are indeed going to shutdown.
    ProcessParameters->OnTerminate.BindLambda([=]()
        {
            UE_LOG(LogShooterGameMode, Log, TEXT("Game Server Process is terminating"));
            // First call ProcessEnding()
            FGameLiftGenericOutcome processEndingOutcome = GameLiftSdkModule->ProcessEnding();
            // Then call Destroy() to free the SDK from memory
            FGameLiftGenericOutcome destroyOutcome = GameLiftSdkModule->Destroy();
            // Exit the process with success or failure
            if (processEndingOutcome.IsSuccess() && destroyOutcome.IsSuccess()) {
                UE_LOG(LogShooterGameMode, Log, TEXT("Server process ending successfully"));
            }
            else {
                if (!processEndingOutcome.IsSuccess()) {
                    const FGameLiftError& error = processEndingOutcome.GetError();
                    UE_LOG(LogShooterGameMode, Error, TEXT("ProcessEnding() failed. Error: %s"),
                        error.m_errorMessage.IsEmpty() ? TEXT("Unknown error") : *error.m_errorMessage);
                }
                if (!destroyOutcome.IsSuccess()) {
                    const FGameLiftError& error = destroyOutcome.GetError();
                    UE_LOG(LogShooterGameMode, Error, TEXT("Destroy() failed. Error: %s"),
                        error.m_errorMessage.IsEmpty() ? TEXT("Unknown error") : *error.m_errorMessage);
                }
            }
        });

    //This is the HealthCheck callback.
    //Amazon GameLift Servers will invoke this callback every 60 seconds or so.
    //Here, a game server might want to check the health of dependencies and such.
    //Simply return true if healthy, false otherwise.
    //The game server has 60 seconds to respond with its health status. Amazon GameLift Servers will default to 'false' if the game server doesn't respond in time.
    //In this case, we're always healthy!
    ProcessParameters->OnHealthCheck.BindLambda([]()
        {
            UE_LOG(LogShooterGameMode, Log, TEXT("Performing Health Check"));
            return true;
        });

    //GameServer.exe -port=7777 LOG=server.mylog
    ProcessParameters->port = FURL::UrlConfig.DefaultPort;
    TArray<FString> CommandLineTokens;
    TArray<FString> CommandLineSwitches;

    FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

    for (FString SwitchStr : CommandLineSwitches)
    {
        FString Key;
        FString Value;

        if (SwitchStr.Split("=", &Key, &Value))
        {
            if (Key.Equals("port"))
            {
                ProcessParameters->port = FCString::Atoi(*Value);
            }
        }
    }

    //Here, the game server tells Amazon GameLift Servers where to find game session log files.
    //At the end of a game session, Amazon GameLift Servers uploads everything in the specified 
    //location and stores it in the cloud for access later.
    TArray<FString> Logfiles;
    Logfiles.Add(TEXT("GameLiftUnrealApp/Saved/Logs/server.log"));
    ProcessParameters->logParameters = Logfiles;

    //The game server calls ProcessReady() to tell Amazon GameLift Servers it's ready to host game sessions.
    UE_LOG(LogShooterGameMode, Log, TEXT("Calling Process Ready..."));
    FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(*ProcessParameters);

    if (ProcessReadyOutcome.IsSuccess())
    {
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_GREEN);
        UE_LOG(LogShooterGameMode, Log, TEXT("Process Ready!"));
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }
    else
    {
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_RED);
        UE_LOG(LogShooterGameMode, Log, TEXT("ERROR: Process Ready Failed!"));
        FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
        UE_LOG(LogShooterGameMode, Log, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
        UE_LOG(LogShooterGameMode, SetColor, TEXT("%s"), COLOR_NONE);
    }

    UE_LOG(LogShooterGameMode, Log, TEXT("InitGameLift completed!"));
#endif
}



