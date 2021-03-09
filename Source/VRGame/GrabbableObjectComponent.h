
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabbableObjectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootingReleasedDelegate);

UENUM()
	enum EGrabbableTypeEnum
	{
		Null,
		EWeapon,
		EMagazine,
	};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRGAME_API UGrabbableObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrabbableObjectComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EGrabbableTypeEnum GrabbableType = EGrabbableTypeEnum::Null;

	FShootDelegate OnShootDelegate;
	FShootingReleasedDelegate OnShootingReleasedDelegate;
		
};
