

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
	RagdollKey = "Ragdoll";
	SensingPawnKey = "IsPawnInSight";
	SensingRagdollKey = "IsRagdollInSight";
	PlayerLocationKey = "PlayerLocation";
	CurrentPlayerLocationKey = "CurrentPlayerLocation";
	RandomLocationKey = "RandomLocation";
	RandomLocationNearPlayerKey = "RandomLocationNearPlayer";
	LookForPlayerKey = "LookForPlayer";
	DefendSelfKey = "DefendSelf";
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
		BlackboardComp->SetValueAsBool(SensingPawnKey, IsPawnSeen);
	}
}

void APatrolAIController::SetPlayerLocation(FVector PlayerLoc)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsVector(PlayerLocationKey, PlayerLoc);
	}
}

void APatrolAIController::SetCurrentPlayerLocation(FVector CurrentPlayerLoc)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsVector(CurrentPlayerLocationKey, CurrentPlayerLoc);
	}
}

void APatrolAIController::SetRandomLocation(FVector RandomLoc)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsVector(RandomLocationKey, RandomLoc);
	}
}

void APatrolAIController::SetRandomLocationNearPlayer(FVector RandomLocNearPlayer)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsVector(RandomLocationNearPlayerKey, RandomLocNearPlayer);
	}
}

void APatrolAIController::SetLookForPlayer(bool LookFor)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsBool(LookForPlayerKey, LookFor);
	}
}

void APatrolAIController::SetDefendSelf(bool DefendSelf)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsBool(DefendSelfKey, DefendSelf);
	}
}

void APatrolAIController::SetRagdollSeen(APawn* SeenRagdoll)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(RagdollKey, SeenRagdoll);
	}
}

void APatrolAIController::SetIsRagdollInSight(bool IsRagdollSeen)
{
	if (Blackboard)
	{
		BlackboardComp->SetValueAsBool(SensingRagdollKey, IsRagdollSeen);
	}
}

FVector APatrolAIController::GetRandomLocation()
{
	float XCoordinate = FMath::FRandRange(200, 1200);
	float YCoordinate = FMath::FRandRange(200, 1200);

	FVector RandomLocation = FVector(XCoordinate, YCoordinate, 50);
	return RandomLocation;
}

FVector APatrolAIController::GetRandomLocationNearPlayer(FVector PlayerLoc)
{
	float PlayerLocX = PlayerLoc.X;
	float PlayerLocY = PlayerLoc.Y;
	float XCoordinate = FMath::FRandRange(PlayerLocX - 500, PlayerLocX + 500);
	float YCoordinate = FMath::FRandRange(PlayerLocY - 500, PlayerLocY + 500);

	FVector RandLocNearPlayer = FVector(XCoordinate, YCoordinate, 50);
	return RandLocNearPlayer;
}

UBlackboardComponent* APatrolAIController::GetBlackboardComp()
{
	return BlackboardComp;
}

TArray<AActor*> APatrolAIController::GetPatrolPoints()
{
	return PatrolPoints;
}
