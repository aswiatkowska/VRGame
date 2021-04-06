
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hand.h"
#include "Weapon.h"
#include "PatrolAI.h"
#include "InventoryObjectTypes.h"
#include "Inventory.h"
#include "GrabbableObjectComponent.h"
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
	class UMotionControllerComponent* RightMotionController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARecastNavMesh* navmesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AHand> HandClass;

	bool GetFromInventory(EInventoryObjectTypes Type);

	void AddToInventory(EInventoryObjectTypes Type);

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

	void ObjectGrabRight();

	void ObjectReleaseRight();

	void ObjectGrabLeft();

	void ObjectReleaseLeft();

	void ShootRight();

	void ShootingReleasedRight();

	void ShootLeft();

	void ShootingReleasedLeft();

	FHitResult hit;

	FVector vector = FVector(1000, 1000, 1000);

	bool SwitchMotion;

	bool CanTeleport;

	APlayerController* playerController;

	AHand* RightHand;

	AHand* LeftHand;

	AInventory* InvMap;

	AWeapon* Weapon;

	APatrolAI* PatrolAI;
};
