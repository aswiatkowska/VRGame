
#include "BTShootPlayer.h"
#include "PatrolAI.h"
#include "PatrolAIController.h"
#include "CollisionQueryParams.h"
#include "Kismet/GameplayStatics.h"


EBTNodeResult::Type UBTShootPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(OwnerComp.GetAIOwner());

	if (ControllerAI)
	{
		UBlackboardComponent* BlackboardComp = ControllerAI->GetBlackboardComp();
		APatrolAI* PatrolAI = Cast<APatrolAI>(ControllerAI->GetPawn());

		if (!PatrolAI->Weapon->IsHeldByPlayer)
		{
			PatrolAI->Weapon->Shoot();
		}
		else
		{
			return EBTNodeResult::Failed;
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

