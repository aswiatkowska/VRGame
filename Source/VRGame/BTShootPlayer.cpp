
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
		
		FHitResult hitShoot;
		const float PistolRange = 2000.0f;
		const FVector Start = PatrolAI->Barrel->GetComponentLocation();
		const FVector End = (PatrolAI->Barrel->GetForwardVector() * PistolRange) + Start;

		FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(PistolRange), false, nullptr);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PatrolAI->ImpactParticles, FTransform(PatrolAI->Barrel->GetComponentRotation(), 
			PatrolAI->Barrel->GetComponentLocation()));

		if (GetWorld()->LineTraceSingleByChannel(hitShoot, Start, End, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PatrolAI->ImpactParticles, FTransform(hitShoot.ImpactNormal.Rotation(), hitShoot.ImpactPoint));
		}

		GetWorld()->SpawnActor<AActor>(PatrolAI->BulletSubclass, Start, PatrolAI->Barrel->GetComponentRotation());

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

