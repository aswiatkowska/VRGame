
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PatrolAI.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "PatrolAIController.generated.h"


UCLASS()
class VRGAME_API APatrolAIController : public AAIController
{
	GENERATED_BODY()

public:
	APatrolAIController();

	void SetPlayerCaught(APawn* CaughtPawn);

	void SetPlayerNotCaught();

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName DestinationKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName PlayerKey;

	UBlackboardComponent* GetBlackboardComp();

	TArray<AActor*> GetPatrolPoints();

	int CurrentPatrolPoint = 0;

private:
	virtual void OnPossess(APawn* InPawn) override;

	UBlackboardComponent* BlackboardComp;

	UBehaviorTreeComponent* BehaviorTreeComp;

	TArray<AActor*> PatrolPoints;

	APatrolAI* CharacterAI = nullptr;
	
};
