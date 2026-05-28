// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace DedicatedServersTags
{
	namespace GameSessionsAPI
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ListFleets);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(FindOrCreateGameSession);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(CreatePlayerSession);
	}
}