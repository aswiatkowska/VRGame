
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
	class UStaticMeshComponent* PhysicalHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPhysicsHandleComponent* PhysicsHandle;

	void ObjectGrab();

	void ObjectRelease();

	void ForceRelease();

	UGrabbableObjectComponent* GetGrabbedObject();

	void SetupHand(EHandEnum RightOrLeft, AHand* OppositeHand);

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool IsAnyObjectGrabbed();

	UFUNCTION()
	void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TMap<UGrabbableObjectComponent*, float> overlapMap;

	UGrabbableObjectComponent* GrabbedObjectGrabbableComponent = nullptr;

	AActor* GrabbedActor = nullptr;

	AHand* OtherHand;

};
