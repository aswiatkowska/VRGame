
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "Magazine.h"
#include "GrabbableObjectComponent.h"
#include "Hand.generated.h"

UCLASS()
class VRGAME_API AHand : public AActor
{
	GENERATED_BODY()
	
public:	
	AHand();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* RightHandSkeletal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* GrabPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionSphere;

	void ObjectGrabRelease();

	void Shoot();

	void ShootingReleased();

	UGrabbableObjectComponent* GetGrabbedObject();

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool IsAnyObjectGrabbed();

	bool CanGrab();

	UFUNCTION()
	void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	AWeapon* Weapon = nullptr;

	AMagazine* Magazine = nullptr;

	UGrabbableObjectComponent* GrabbedObjectGrabbableComponent = nullptr;

	UGrabbableObjectComponent* DetectedGrabbable;

	AActor* GrabbedActor;

};
