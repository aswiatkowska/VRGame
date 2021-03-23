

#include "PatrolAIController.h"
#include "PatrolAIPoint.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

APatrolAIController::APatrolAIController()
{
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTree");
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");

	DestinationKey = "Destination";
	PlayerKey = "Target";
	SensingKey = "IsPawnInSight";
	PlayerLocationKey = "PlayerLocation";
}

void APatrolAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CharacterAI = Cast<APatrolAI>(InPawn);

	if (CharacterAI)
	{
		if (CharacterAI->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*CharacterAI->BehaviorTree->BlackboardAsset);
		}
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolAIPoint::StaticClass(), PatrolPoints);

	BehaviorTreeComp->StartTree(*CharacterAI->BehaviorTree);
}

void APatrolAIController::SetPlayerCaught(APawn* CaughtPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(PlayerKey, CaughtPawn);
	}
}

void APatrolAIController::SetPlayerNotCaught()
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(PlayerKey, nullptr);
	}
}

void APatrolAIController::SetIsPawnInSight(bool IsPawnSeen)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsBool(SensingKey, IsPawnSeen);
	}
}

void APatrolAIController::SetPlayerLocation(FVector PlayerLoc)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsVector(PlayerLocationKey, PlayerLoc);
	}
}

UBlackboardComponent* APatrolAIController::GetBlackboardComp()
{
	return BlackboardComp;
}

TArray<AActor*> APatrolAIController::GetPatrolPoints()
{
	return PatrolPoints;
}
