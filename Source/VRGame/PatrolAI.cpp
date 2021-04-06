

#include "PatrolAI.h"
#include "Bullet.h"
#include "PatrolAIController.h"
#include "CustomChannels.h"
#include "Kismet/GameplayStatics.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 


APatrolAI::APatrolAI()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	PawnSensingComp->SetPeripheralVisionAngle(20.0f);
	GetCharacterMovement()->MaxWalkSpeed = 100;

	GetMesh()->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::PatrolAI));
	GetMesh()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Overlap);

	GrabbableObjComp = CreateDefaultSubobject<UGrabbableObjectComponent>("GrabbableObjComp");

	OnActorBeginOverlap.AddDynamic(this, &APatrolAI::OnOverlap);

}

void APatrolAI::BeginPlay()
{
	Super::BeginPlay();

	GrabbableObjComp->GrabbableType = EGrabbableTypeEnum::ERagdoll;

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

void APatrolAI::OnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Cast<ABullet>(OtherActor) != nullptr)
	{
		if (CurrentWeaponType == EWeaponTypeEnum::EGun)
		{
			NumberOfLifes = NumberOfLifes - 1;
		}
		else if (CurrentWeaponType == EWeaponTypeEnum::ERifle)
		{
			NumberOfLifes = NumberOfLifes - 2;
		}

		if (NumberOfLifes == 0)
		{
			GetCharacterMovement()->DisableMovement();
			GetMesh()->SetAllBodiesSimulatePhysics(true);
		}
	}
}

void APatrolAI::ChangeCurrentWeaponType(TEnumAsByte<EWeaponTypeEnum> Type)
{
	CurrentWeaponType = Type;
}

