
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolAI.generated.h"


UCLASS()
class VRGAME_API APatrolAI : public ACharacter
{
	GENERATED_BODY()

public:
	APatrolAI();

	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGrabbableObjectComponent* RightHandGrabbable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGrabbableObjectComponent* LeftHandGrabbable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGrabbableObjectComponent* RightLegGrabbable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGrabbableObjectComponent* LeftLegGrabbable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* RightHandSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* LeftHandSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* RightLegSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* LeftLegSphere;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnBulletOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerCaught(APawn* CaughtPawn);

	void OnPlayerNotCaught();

	void StopLookingForPlayer();

	bool IsPawnInSight;

	bool LookForPlayer = false;

	int NumberOfLifes = 100;

	FVector PlayerLoc;

	APawn* PlayerPawn;

};
