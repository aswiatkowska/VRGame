

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
	RightHandSphere = CreateDefaultSubobject<USphereComponent>("RightHandSphere");
	RightHandSphere->SetupAttachment(GetMesh(), FName(TEXT("hand_r")));
	RightHandSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	RightHandSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);

	LeftHandSphere = CreateDefaultSubobject<USphereComponent>("LeftHandSphere");
	LeftHandSphere->SetupAttachment(GetMesh(), FName(TEXT("hand_l")));
	LeftHandSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	LeftHandSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);

	RightLegSphere = CreateDefaultSubobject<USphereComponent>("RightLegSphere");
	RightLegSphere->SetupAttachment(GetMesh(), FName(TEXT("foot_r")));
	RightLegSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	RightLegSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);

	LeftLegSphere = CreateDefaultSubobject<USphereComponent>("LeftLegSphere");
	LeftLegSphere->SetupAttachment(GetMesh(), FName(TEXT("foot_l")));
	LeftLegSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	LeftLegSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);

	RightHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("RightHandGrabbable");
	RightHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollHand;
	RightHandGrabbable->CollisionComponent = (UPrimitiveComponent*)RightHandSphere;

	LeftHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("LeftHandGrabbable");
	LeftHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollHand;
	LeftHandGrabbable->CollisionComponent = (UPrimitiveComponent*)LeftHandSphere;

	RightLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("RightLegGrabbable");
	RightLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollLeg;
	RightLegGrabbable->CollisionComponent = (UPrimitiveComponent*)RightLegSphere;

	LeftLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("LeftLegGrabbable");
	LeftLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollLeg;
	LeftLegGrabbable->CollisionComponent = (UPrimitiveComponent*)LeftLegSphere;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	PawnSensingComp->SetPeripheralVisionAngle(20.0f);
	GetCharacterMovement()->MaxWalkSpeed = 100;

	GetMesh()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Overlap);
}

void APatrolAI::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &APatrolAI::OnBulletOverlapBegin);

	PlayerPawn = Cast<APawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (PawnSensingComp && !IsPawnInSight)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &APatrolAI::OnPlayerCaught);
	}

	RightHandGrabbable->OnGrabDelegate.AddDynamic(this, &APatrolAI::OnGrab);
	RightHandGrabbable->OnReleaseDelegate.AddDynamic(this, &APatrolAI::OnRelease);

	LeftHandGrabbable->OnGrabDelegate.AddDynamic(this, &APatrolAI::OnGrab);
	LeftHandGrabbable->OnReleaseDelegate.AddDynamic(this, &APatrolAI::OnRelease);

	RightLegGrabbable->OnGrabDelegate.AddDynamic(this, &APatrolAI::OnGrab);
	RightLegGrabbable->OnReleaseDelegate.AddDynamic(this, &APatrolAI::OnGrab);

	LeftLegGrabbable->OnGrabDelegate.AddDynamic(this, &APatrolAI::OnGrab);
	LeftLegGrabbable->OnReleaseDelegate.AddDynamic(this, &APatrolAI::OnRelease);
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
		UE_LOG(LogTemp, Log, TEXT("Overlap begin with %s"), *(OtherActor->GetName()))
		UE_LOG(LogTemp, Log, TEXT("Overlapped with %s"), *(OverlappedComponent->GetName()))
		ABullet* Bullet = Cast<ABullet>(OtherActor);
		NumberOfLifes = NumberOfLifes - Bullet->BulletImpact;

		if (NumberOfLifes == 0)
		{
			GetCharacterMovement()->DisableMovement();
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());
			ControllerAI->UnPossess();
			ControllerAI->Destroy();
		}
	}
}

void APatrolAI::OnGrab()
{
	GetMesh()->SetSimulatePhysics(false);
}

void APatrolAI::OnRelease()
{
	GetMesh()->SetSimulatePhysics(true);
}
