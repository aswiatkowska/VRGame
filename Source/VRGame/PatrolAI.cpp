

#include "PatrolAI.h"
#include "PatrolAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 


APatrolAI::APatrolAI()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	PawnSensingComp->SetPeripheralVisionAngle(20.0f);
	GetCharacterMovement()->MaxWalkSpeed = 100;
}

void APatrolAI::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &APatrolAI::OnPlayerCaught);
	}
}

void APatrolAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PawnSensingComp->CouldSeePawn(SeenPawn))
	{
		OnPlayerNotCaught();
	}
}

void APatrolAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APatrolAI::OnPlayerCaught(APawn* CaughtPawn)
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (ControllerAI)
	{
		ControllerAI->SetPlayerCaught(CaughtPawn);
	}
}

void APatrolAI::OnPlayerNotCaught()
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (ControllerAI)
	{
		ControllerAI->SetPlayerNotCaught();
	}
}


