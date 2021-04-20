
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "PatrolAI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGetHelpDelegate);

UCLASS()
class VRGAME_API APatrolAI : public ACharacter
{
	GENERATED_BODY()

public:
	APatrolAI();

	AWeapon* Weapon;

	FGetHelpDelegate OnGetHelpDelegate;

	void StopLookingForPlayer();

	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = AI)
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponSubclass;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int cooldownTimePatrolMultiplier;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnBulletOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	void OnPlayerNotSeen();

	void SetRagdollNotSeen();

	void StopDefendingSelf();

	bool IsPawnInSight;

	bool IsRagdollInSight;

	bool LookForPlayer = false;

	bool IsDead = false;

	int NumberOfLifes = 100;

	FVector PlayerLoc;

	FVector CurrentPlayerLoc;

	APawn* PlayerPawn;
};
