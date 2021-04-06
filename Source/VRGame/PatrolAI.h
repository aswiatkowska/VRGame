
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GrabbableObjectComponent.h"
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
	class UGrabbableObjectComponent* GrabbableObjComp;

	void ChangeCurrentWeaponType(TEnumAsByte<EWeaponTypeEnum> Type);

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnPlayerCaught(APawn* CaughtPawn);

	void OnPlayerNotCaught();

	void StopLookingForPlayer();

	bool IsPawnInSight;

	bool LookForPlayer = false;

	int NumberOfLifes = 4;

	FVector PlayerLoc;

	TEnumAsByte<EWeaponTypeEnum> CurrentWeaponType;

	APawn* PlayerPawn;

};
