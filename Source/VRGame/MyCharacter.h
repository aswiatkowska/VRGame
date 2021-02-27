
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h"
#include "Gun.h"
#include "MyCharacter.generated.h"

UCLASS()
class VRGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UMotionControllerComponent* LeftMotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UMotionControllerComponent* RightMotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* GrabPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* LeftHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* RightHandSkeletal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARecastNavMesh* navmesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionSphere;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Teleport();

	bool TeleportLocation();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void MoveBackwards(float Value);

	void MoveLeft(float Value);

	void ChangeMotion();

	void TurnRight();

	void TurnLeft();

	void GrabWeapon();

	void ReleaseWeapon();

	void Shoot();

	void ShootingReleased();

	FHitResult hit;

	FVector vector = FVector(1000, 1000, 1000);

	bool SwitchMotion;

	bool CanTeleport;

	bool CanGrab;

	AWeapon* Weapon = nullptr;

	UFUNCTION()
	void OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool WeaponGrabbed = false;

	APlayerController* playerController;
};
