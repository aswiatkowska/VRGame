

#include "PatrolAI.h"
#include "PatrolAIController.h"
#include "Kismet/GameplayStatics.h" 
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

	PlayerPawn = Cast<APawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (PawnSensingComp && !IsPawnInSight)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &APatrolAI::OnPlayerCaught);
	}
}

void APatrolAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPawnInSight)
	{
		if (!PawnSensingComp->CouldSeePawn(PlayerPawn))
		{
			IsPawnInSight = false;
			OnPlayerNotCaught();
		}
	}
}

void APatrolAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APatrolAI::OnPlayerCaught(APawn* CaughtPawn)
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (ControllerAI && CaughtPawn == PlayerPawn)
	{
		ControllerAI->SetPlayerCaught(CaughtPawn);
		IsPawnInSight = true;
		ControllerAI->SetIsPawnInSight(IsPawnInSight);
	}
}

void APatrolAI::OnPlayerNotCaught()
{
	if (IsPawnInSight)
	{
		return;
	}

	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (ControllerAI)
	{
		ControllerAI->SetPlayerNotCaught();
		PlayerLoc = PlayerPawn->GetActorLocation();
		ControllerAI->SetPlayerLocation(PlayerLoc);
		LookForPlayer = true;
		ControllerAI->SetLookForPlayer(LookForPlayer);
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &APatrolAI::StopLookingForPlayer, 6);
		ControllerAI->SetIsPawnInSight(IsPawnInSight);
	}
}

void APatrolAI::StopLookingForPlayer()
{
	LookForPlayer = false;
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());
	if (ControllerAI)
	{
		ControllerAI->SetLookForPlayer(LookForPlayer);
	}
}


