// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_GameInstanceSubsystem.h"
#include "DedicatedServers/DedicatedServers.h"

UDS_GameInstanceSubsystem::UDS_GameInstanceSubsystem()
{
	bGameLiftInitialized = false;
}

#if WITH_GAMELIFT
void UDS_GameInstanceSubsystem::InitGameLift(const FServerParameters& ServerParams)
{
    if (bGameLiftInitialized)
    {
        return;
    }

    UE_LOG(LogDedicatedServers, Log, TEXT("Calling InitGameLift..."));

    // 1. GameLift SDK 모듈 로드
    FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

    // 2. GameLift Anywhere 환경 검사 및 파라미터 설정
    FServerParameters ServerParametersForAnywhere = ServerParams;
    bool bIsAnywhereActive = false;
    if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
    {
        bIsAnywhereActive = true;
    }

    UE_LOG(LogDedicatedServers, Log, TEXT("Initializing the GameLift Server..."));

    FGameLiftGenericOutcome InitSdkOutcome = GameLiftSdkModule->InitSDK(ServerParametersForAnywhere);
    if (InitSdkOutcome.IsSuccess())
    {
        UE_LOG(LogDedicatedServers, Log, TEXT("GameLift InitSDK succeeded!"));
    }
    else
    {
        UE_LOG(LogDedicatedServers, Error, TEXT("ERROR: InitSDK failed : ("));
        FGameLiftError GameLiftError = InitSdkOutcome.GetError();
        UE_LOG(LogDedicatedServers, Error, TEXT("ERROR: %s"), *GameLiftError.m_errorMessage);
        return;
    }

    // 3. 콜백 바인딩 (OnStartGameSession)
    ProcessParameters.OnStartGameSession.BindLambda([=](Aws::GameLift::Server::Model::GameSession InGameSession)
        {
            FString GameSessionId = FString(InGameSession.GetGameSessionId());
            UE_LOG(LogDedicatedServers, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
            GameLiftSdkModule->ActivateGameSession();
        });

    // 4. 콜백 바인딩 (OnTerminate)
    ProcessParameters.OnTerminate.BindLambda([=]()
        {
            UE_LOG(LogDedicatedServers, Log, TEXT("Game Server Process is terminating"));

            FGameLiftGenericOutcome processEndingOutcome = GameLiftSdkModule->ProcessEnding();
            FGameLiftGenericOutcome destroyOutcome = GameLiftSdkModule->Destroy();

            if (processEndingOutcome.IsSuccess() && destroyOutcome.IsSuccess())
            {
                UE_LOG(LogDedicatedServers, Log, TEXT("Server process ending successfully"));
            }
            else
            {
                if (!processEndingOutcome.IsSuccess())
                {
                    const FGameLiftError& error = processEndingOutcome.GetError();
                    UE_LOG(LogDedicatedServers, Error, TEXT("ProcessEnding() failed. Error: %s"),
                        error.m_errorMessage.IsEmpty() ? TEXT("Unknown error") : *error.m_errorMessage);
                }
                if (!destroyOutcome.IsSuccess())
                {
                    const FGameLiftError& error = destroyOutcome.GetError();
                    UE_LOG(LogDedicatedServers, Error, TEXT("Destroy() failed. Error: %s"),
                        error.m_errorMessage.IsEmpty() ? TEXT("Unknown error") : *error.m_errorMessage);
                }
            }
        });

    // 5. 콜백 바인딩 (OnHealthCheck)
    ProcessParameters.OnHealthCheck.BindLambda([]()
        {
            UE_LOG(LogDedicatedServers, Log, TEXT("Performing Health Check"));
            return true;
        });

    // 6. 포트 및 로그 경로 설정
    int32 Port = FURL::UrlConfig.DefaultPort;
    ParseCommandLinePort(Port);

    TArray<FString> Logfiles;
    Logfiles.Add(TEXT("GameLiftUnrealApp/Saved/Logs/FPSTemplate.log"));
    ProcessParameters.logParameters = Logfiles;

    UE_LOG(LogDedicatedServers, Log, TEXT("Calling Process Ready..."));
    FGameLiftGenericOutcome ProcessReadyOutcome = GameLiftSdkModule->ProcessReady(ProcessParameters);

    if (ProcessReadyOutcome.IsSuccess())
    {
        UE_LOG(LogDedicatedServers, Log, TEXT("Process Ready!"));
    }
    else
    {
        UE_LOG(LogDedicatedServers, Error, TEXT("ERROR: Process Ready Failed!"));
        FGameLiftError ProcessReadyError = ProcessReadyOutcome.GetError();
        UE_LOG(LogDedicatedServers, Error, TEXT("ERROR: %s"), *ProcessReadyError.m_errorMessage);
    }

    UE_LOG(LogDedicatedServers, Log, TEXT("InitGameLift completed!"));
    bGameLiftInitialized = true;
}
#endif

void UDS_GameInstanceSubsystem::ParseCommandLinePort(int32& OutPort)
{
    int32 Port = FURL::UrlConfig.DefaultPort;
    FParse::Value(FCommandLine::Get(), TEXT("port="), Port);
    OutPort = Port;

#if WITH_GAMELIFT
    ProcessParameters.port = Port;
#endif
}