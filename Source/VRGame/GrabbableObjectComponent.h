
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabbableObjectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGrabDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReleaseDelegate);

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

	FGrabDelegate OnGrabDelegate;

	FReleaseDelegate OnReleaseDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RightRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator LeftRotation = FRotator::ZeroRotator;
		
};
