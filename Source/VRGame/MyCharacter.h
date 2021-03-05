
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hand.h"
#include "MyCharacter.generated.h"

UCLASS()
class VRGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UMotionControllerComponent* LeftMotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* LeftHandSkeletal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARecastNavMesh* navmesh;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Teleport();

	bool TeleportLocation();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void MoveBackwards(float Value);

	void MoveLeft(float Value);

	void ChangeMotion();

	void TurnRight();

	void TurnLeft();

	void WeaponGrabRelease();

	void Shoot();

	void ShootingReleased();

	FHitResult hit;

	FVector vector = FVector(1000, 1000, 1000);

	bool SwitchMotion;

	bool CanTeleport;

	AHand* Hand;

	APlayerController* playerController;
};
