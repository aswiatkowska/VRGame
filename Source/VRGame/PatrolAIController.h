
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

	void SetIsPawnInSight(bool IsPawnSeen);

	void SetPlayerLocation(FVector PlayerLoc);

	void SetRandomLocation(FVector RandomLoc);

	void SetLookForPlayer(bool LookFor);

	void SetRagdollSeen(APawn* SeenRagdoll);

	void SetIsRagdollInSight(bool IsRagdollSeen);

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName DestinationKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName PlayerKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName RagdollKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName SensingPawnKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName SensingRagdollKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName PlayerLocationKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName RandomLocationKey;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	FName LookForPlayerKey;

	UBlackboardComponent* GetBlackboardComp();

	TArray<AActor*> GetPatrolPoints();

	FVector GetRandomLocation();

	int CurrentPatrolPoint = 0;

private:
	virtual void OnPossess(APawn* InPawn) override;

	UBlackboardComponent* BlackboardComp;

	UBehaviorTreeComponent* BehaviorTreeComp;

	TArray<AActor*> PatrolPoints;

	APatrolAI* CharacterAI = nullptr;
	
};
