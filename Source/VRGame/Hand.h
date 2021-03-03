
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hand.generated.h"

UCLASS()
class VRGAME_API AHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AHand();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
