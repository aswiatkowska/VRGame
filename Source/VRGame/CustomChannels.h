
#pragma once
#include "Engine/EngineTypes.h"
#include "CustomChannels.generated.h"

enum class CustomCollisionChannelsEnum
{
	Bullet = ECollisionChannel::ECC_GameTraceChannel1,
	Hand = ECollisionChannel::ECC_GameTraceChannel2,
	GrabbableObject = ECollisionChannel::ECC_GameTraceChannel3,
	HandPhysical = ECollisionChannel::ECC_GameTraceChannel4
};

USTRUCT()
struct FAyStruct
{
	GENERATED_BODY()

};
