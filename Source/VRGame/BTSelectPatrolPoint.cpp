
#include "BTSelectPatrolPoint.h"
#include "PatrolAIPoint.h"
#include "PatrolAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTSelectPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(OwnerComp.GetAIOwner());

	if (ControllerAI)
	{
		UBlackboardComponent* BlackboardComp = ControllerAI->GetBlackboardComp();
		APatrolAIPoint* CurrentPoint = Cast<APatrolAIPoint>(BlackboardComp->GetValueAsObject("Destination"));

		TArray<AActor*> AvaliablePatrolPoints = ControllerAI->GetPatrolPoints();
		APatrolAIPoint* NextPatrolPoint = nullptr;

		if (ControllerAI->CurrentPatrolPoint != AvaliablePatrolPoints.Num() - 1)
		{
			NextPatrolPoint = Cast<APatrolAIPoint>(AvaliablePatrolPoints[++ControllerAI->CurrentPatrolPoint]);
		}
		else
		{
			NextPatrolPoint = Cast<APatrolAIPoint>(AvaliablePatrolPoints[0]);
			ControllerAI->CurrentPatrolPoint = 0;
		}

		BlackboardComp->SetValueAsObject("Destination", NextPatrolPoint);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

