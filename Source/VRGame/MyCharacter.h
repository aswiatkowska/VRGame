
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	class UStaticMeshComponent* LeftHandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* RightHandSkeletal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ARecastNavMesh* navmesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* ImpactParticles;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Teleport();

	void Shoot();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void ChangeMotion();

	void DrawDebugLine();

	void ClearDebugLine();

	FHitResult hit;

	FVector vector = FVector(1000, 1000, 1000);

	bool SwitchMotion;
};
