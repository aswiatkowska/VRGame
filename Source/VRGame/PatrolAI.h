
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

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnPlayerCaught(APawn* CaughtPawn);

	void OnPlayerNotCaught();

	APawn* SeenPawn;

};
