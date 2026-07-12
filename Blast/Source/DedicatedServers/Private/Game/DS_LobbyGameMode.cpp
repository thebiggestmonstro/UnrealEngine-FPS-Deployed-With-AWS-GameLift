// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/Game/DS_LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Game/DS_GameInstanceSubsystem.h"
#include "Player/DSPlayerController.h"

#if WITH_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#endif

#include "GenericPlatform/GenericPlatformOutputDevices.h"

DEFINE_LOG_CATEGORY(LogDS_LobbyGameMode);

ADS_LobbyGameMode::ADS_LobbyGameMode()
{
    bUseSeamlessTravel = true;
    LobbyStatus = ELobbyStatus::WaitingForPlayers;
    MinPlayers = 1;
    LobbyCountdownTimer.Type = ECountdownTimerType::LobbyCountdown;
}

void ADS_LobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

    const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
    const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

    TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
}

FString ADS_LobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
    FString InitializedString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

    const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
    const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

    if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(NewPlayerController); IsValid(DSPlayerController))
    {
        DSPlayerController->PlayerSessionId = PlayerSessionId;
        DSPlayerController->Username = Username;
    }

    return InitializedString;
}

void ADS_LobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (GetNumPlayers() >= MinPlayers && LobbyStatus == ELobbyStatus::WaitingForPlayers)
    {
        LobbyStatus = ELobbyStatus::CountdownToSeamlessTravel;

        StartCountdownTimer(LobbyCountdownTimer);
    }
}

void ADS_LobbyGameMode::BeginPlay()
{
    Super::BeginPlay();

#if WITH_GAMELIFT
    InitGameLift();
#endif
}

void ADS_LobbyGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
    Super::OnCountdownTimerFinished(Type);

    if (Type == ECountdownTimerType::LobbyCountdown)
    {
        StopCountdownTimer(LobbyCountdownTimer);
        LobbyStatus = ELobbyStatus::SeamlessTravelling;

        if (!MapToTravelTo.IsNull())
        {
            TrySeamlessTravel(MapToTravelTo);
        }
    }
}

void ADS_LobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    CancelCountdown();

    RemovePlayerSession(Exiting);
}

void ADS_LobbyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
    Super::InitSeamlessTravelPlayer(NewController);

    CancelCountdown();
}

void ADS_LobbyGameMode::CancelCountdown()
{
    if (GetNumPlayers() - 1 < MinPlayers && LobbyStatus == ELobbyStatus::CountdownToSeamlessTravel)
    {
        LobbyStatus = ELobbyStatus::WaitingForPlayers;
        StopCountdownTimer(LobbyCountdownTimer);
    }
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

void ADS_LobbyGameMode::TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage)
{
    if (PlayerSessionId.IsEmpty() || Username.IsEmpty())
    {
        OutErrorMessage = TEXT("PlayerSessionId and/or Username invalid.");
        return;
    }

#if WITH_GAMELIFT
    Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
    DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

    const auto& DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
    if (!DescribePlayerSessionsOutcome.IsSuccess())
    {
        OutErrorMessage = TEXT("DescribePlayerSessions failed.");
        return;
    }

    const auto& DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
    int32 Count = 0;
    const Aws::GameLift::Server::Model::PlayerSession* PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
    if (PlayerSessions == nullptr || Count == 0)
    {
        OutErrorMessage = TEXT("GetPlayerSessions failed.");
        return;
    }

    for (int32 i = 0; i < Count; i++)
    {
        const Aws::GameLift::Server::Model::PlayerSession& PlayerSession = PlayerSessions[i];
        if (!Username.Equals(PlayerSession.GetPlayerId()))
        {
            continue;
        }

        if (PlayerSession.GetStatus() != Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED)
        {
            OutErrorMessage = FString::Printf(TEXT("Session for %s not RESERVED; Fail PreLogin."), *Username);
            return;
        }

        const auto& AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
        OutErrorMessage = AcceptPlayerSessionOutcome.IsSuccess() ? "" : FString::Printf(TEXT("Failed to accept player session for %s"), *Username);
    }
#endif
}

void ADS_LobbyGameMode::InitGameLift()
{
#if WITH_GAMELIFT
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (DSGameInstanceSubsystem = GameInstance->GetSubsystem<UDS_GameInstanceSubsystem>(); IsValid(DSGameInstanceSubsystem))
        {
            FServerParameters ServerParameters;

            if (FParse::Param(FCommandLine::Get(), TEXT("glAnywhere")))
            {
                SetServerParameters(ServerParameters);
            }

            DSGameInstanceSubsystem->InitGameLift(ServerParameters);
        }
    }
#endif
}