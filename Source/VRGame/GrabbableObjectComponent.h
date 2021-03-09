
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabbableObjectComponent.generated.h"

UENUM(BlueprintType)
enum class ObjectToGrab : uint8
{
	WEAPON = 0,
	MAGAZINE = 1
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRGAME_API UGrabbableObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabbableObjectComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
