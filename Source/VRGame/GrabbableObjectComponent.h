
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabbableObjectComponent.generated.h"

UENUM()
namespace EObjectToGrab
{
	enum Type
	{
		Weapon,
		Magazine,
	};
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRGAME_API UGrabbableObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabbableObjectComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
