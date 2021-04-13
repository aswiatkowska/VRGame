

#include "PatrolAI.h"
#include "Bullet.h"
#include "PatrolAIController.h"
#include "CustomChannels.h"
#include "Kismet/GameplayStatics.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystem.h"
#include "GrabbableObjectComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

	RightHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("hand_r");
	RightHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollHand;
	RightHandGrabbable->CollisionComponent = (UPrimitiveComponent*)RightHandSphere;

	LeftHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("hand_l");
	LeftHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollHand;
	LeftHandGrabbable->CollisionComponent = (UPrimitiveComponent*)LeftHandSphere;

	RightLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("foot_r");
	RightLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollLeg;
	RightLegGrabbable->CollisionComponent = (UPrimitiveComponent*)RightLegSphere;

	LeftLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("foot_l");
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

	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponSubclass, GetMesh()->GetComponentLocation(), FRotator::ZeroRotator);
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon_r"));
	Weapon->UnlimitedBullets = true;
	Weapon->ShootingSpree = false;

	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &APatrolAI::OnBulletOverlapBegin);

	PlayerPawn = Cast<APawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (PawnSensingComp && !IsPawnInSight)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &APatrolAI::OnPawnSeen);
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
			OnPlayerNotSeen();
		}
	}
}

void APatrolAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APatrolAI::OnPawnSeen(APawn* SeenPawn)
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (ControllerAI && SeenPawn == Ragdoll)
	{
		ControllerAI->SetRagdollSeen(SeenPawn);
		IsRagdollInSight = true;
		ControllerAI->SetIsRagdollInSight(IsRagdollInSight);
		ControllerAI->SetRandomLocation(ControllerAI->GetRandomLocation());
	}
	else if (ControllerAI && SeenPawn == PlayerPawn)
	{
		ControllerAI->SetDefendSelf(false);
		ControllerAI->SetPlayerCaught(SeenPawn);
		IsPawnInSight = true;
		ControllerAI->SetIsPawnInSight(IsPawnInSight);
	}
}

void APatrolAI::OnPlayerNotSeen()
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

		if (Bullet->IsActorBeingDestroyed())
		{
			return;
		}

		NumberOfLifes = NumberOfLifes - Bullet->BulletImpact;
		Bullet->OnDestroy();

		APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());
		if (ControllerAI)
		{
			CurrentPlayerLoc = PlayerPawn->GetActorLocation();
			ControllerAI->SetCurrentPlayerLocation(CurrentPlayerLoc);
			ControllerAI->SetDefendSelf(true);
		}

		if (NumberOfLifes == 0)
		{
			GetCharacterMovement()->DisableMovement();
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			ControllerAI->UnPossess();
			ControllerAI->Destroy();
			Ragdoll = Cast<APawn>(this);
			Weapon->UnlimitedBullets = false;
			Weapon->ShootingSpree = true;
		}
	}
}
