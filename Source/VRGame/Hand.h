
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObjectComponent.h"
#include "Hand.generated.h"

UENUM()
enum EHandEnum
{
	ERight,
	ELeft,
};

UCLASS()
class VRGAME_API AHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AHand();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* HandSkeletal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* GrabPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EHandEnum> HandType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsActive;

	void ObjectGrabRelease();

	UGrabbableObjectComponent* GetGrabbedObject();

	AHand* OtherHand;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void ForceRelease();

	bool IsAnyObjectGrabbed();

	UFUNCTION()
	void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TMap<UGrabbableObjectComponent*, float> overlapMap;

	UGrabbableObjectComponent* GrabbedObjectGrabbableComponent = nullptr;

	AActor* GrabbedActor = nullptr;

};
