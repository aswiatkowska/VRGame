

#include "PatrolAI.h"
#include "Bullet.h"
#include "PatrolAIController.h"
#include "CustomChannels.h"
#include "Kismet/GameplayStatics.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "GrabbableObjectComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 


APatrolAI::APatrolAI()
{
	RightHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("RightHandGrabbable");
	RightHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdoll;
	LeftHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("LeftHandGrabbable");
	LeftHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdoll;
	RightLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("RightLegGrabbable");
	RightLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdoll;
	LeftLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("LeftLegGrabbable");
	LeftLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdoll;

	RightHandSphere = CreateDefaultSubobject<USphereComponent>("RightHandSphere");
	LeftHandSphere = CreateDefaultSubobject<USphereComponent>("LeftHandSphere");
	RightLegSphere = CreateDefaultSubobject<USphereComponent>("RightLegSphere");
	LeftLegSphere = CreateDefaultSubobject<USphereComponent>("LeftLegSphere");

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	PawnSensingComp->SetPeripheralVisionAngle(20.0f);
	GetCharacterMovement()->MaxWalkSpeed = 100;
	GetMesh()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Overlap);

	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &APatrolAI::OnBulletOverlapBegin);
}

void APatrolAI::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Overlap);

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

void APatrolAI::OnBulletOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<ABullet>(OtherActor) != nullptr)
	{
		NumberOfLifes = NumberOfLifes - 25;

		if (NumberOfLifes == 0)
		{
			GetCharacterMovement()->DisableMovement();
			GetMesh()->SetAllBodiesSimulatePhysics(true);
		}
	}
}
