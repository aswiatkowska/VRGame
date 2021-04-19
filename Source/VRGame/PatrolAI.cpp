

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
	RightHandGrabbable->IsPatrolAI = true;

	LeftHandGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("hand_l");
	LeftHandGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollHand;
	LeftHandGrabbable->CollisionComponent = (UPrimitiveComponent*)LeftHandSphere;
	LeftHandGrabbable->IsPatrolAI = true;

	RightLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("foot_r");
	RightLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollLeg;
	RightLegGrabbable->CollisionComponent = (UPrimitiveComponent*)RightLegSphere;
	RightLegGrabbable->IsPatrolAI = true;

	LeftLegGrabbable = CreateDefaultSubobject<UGrabbableObjectComponent>("foot_l");
	LeftLegGrabbable->GrabbableType = EGrabbableTypeEnum::ERagdollLeg;
	LeftLegGrabbable->CollisionComponent = (UPrimitiveComponent*)LeftLegSphere;
	LeftLegGrabbable->IsPatrolAI = true;

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
	Weapon->cooldownTime = Weapon->cooldownTime * cooldownTimePatrolMultiplier;

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

	if (ControllerAI && Cast<APatrolAI>(SeenPawn) != nullptr)
	{
		APatrolAI* OtherPatrolAI = Cast<APatrolAI>(SeenPawn);

		if (OtherPatrolAI->IsDead)
		{
			ControllerAI->SetRagdollSeen(SeenPawn);
			IsRagdollInSight = true;
			ControllerAI->SetIsRagdollInSight(IsRagdollInSight);
			ControllerAI->SetRandomLocation(ControllerAI->GetRandomLocation());
			LookForPlayer = true;
			ControllerAI->SetLookForPlayer(LookForPlayer);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &APatrolAI::SetRagdollNotSeen, 3);
			FTimerHandle timerhandle;
			GetWorld()->GetTimerManager().SetTimer(timerhandle, this, &APatrolAI::StopLookingForPlayer, 10);
		}
	}
	else if (ControllerAI && SeenPawn == PlayerPawn)
	{
		StopDefendingSelf();
		ControllerAI->SetPlayerCaught(SeenPawn);
		IsPawnInSight = true;
		ControllerAI->SetIsPawnInSight(IsPawnInSight);
		PlayerLoc = PlayerPawn->GetActorLocation();
		ControllerAI->SetPlayerLocation(PlayerLoc);
		ControllerAI->SetRandomLocationNearPlayer(ControllerAI->GetRandomLocationNearPlayer(PlayerLoc));
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
		IsPawnInSight = false;
		ControllerAI->SetIsPawnInSight(IsPawnInSight);
		PlayerLoc = PlayerPawn->GetActorLocation();
		ControllerAI->SetPlayerLocation(PlayerLoc);
		ControllerAI->SetRandomLocationNearPlayer(ControllerAI->GetRandomLocationNearPlayer(PlayerLoc));
		LookForPlayer = true;
		ControllerAI->SetLookForPlayer(LookForPlayer);
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &APatrolAI::StopLookingForPlayer, 6);
	}
}

void APatrolAI::StopLookingForPlayer()
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (LookForPlayer)
	{
		LookForPlayer = false;

		if (ControllerAI)
		{
			ControllerAI->SetLookForPlayer(LookForPlayer);
		}
	}
}

void APatrolAI::SetRagdollNotSeen()
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (IsRagdollInSight)
	{
		IsRagdollInSight = false;
		ControllerAI->SetIsRagdollInSight(IsRagdollInSight);
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
			PlayerLoc = PlayerPawn->GetActorLocation();
			ControllerAI->SetPlayerLocation(PlayerLoc);
			ControllerAI->SetRandomLocationNearPlayer(ControllerAI->GetRandomLocationNearPlayer(PlayerLoc));

			ControllerAI->SetDefendSelf(true);
			LookForPlayer = true;
			ControllerAI->SetLookForPlayer(LookForPlayer);

			FTimerHandle timerhandle;
			GetWorld()->GetTimerManager().SetTimer(timerhandle, this, &APatrolAI::StopLookingForPlayer, 10);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &APatrolAI::StopDefendingSelf, 5);

		}

		if (NumberOfLifes == 0)
		{
			RightHandGrabbable->IsPatrolAIDead = true;
			RightLegGrabbable->IsPatrolAIDead = true;
			LeftHandGrabbable->IsPatrolAIDead = true;
			LeftLegGrabbable->IsPatrolAIDead = true;
			GetCharacterMovement()->DisableMovement();
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			ControllerAI->UnPossess();
			ControllerAI->Destroy();
			IsDead = true;

			if (!Weapon->IsHeldByPlayer)
			{
				Weapon->DetachAllSceneComponents(GetMesh(), FDetachmentTransformRules::KeepWorldTransform);
				Weapon->ShootingReleased();
				Weapon->WeaponMesh->SetSimulatePhysics(true);
				Weapon->UnlimitedBullets = false;
				Weapon->ShootingSpree = true;
				Weapon->cooldownTime = Weapon->cooldownTime / cooldownTimePatrolMultiplier;
			}
		}
	}
}

void APatrolAI::StopDefendingSelf()
{
	APatrolAIController* ControllerAI = Cast<APatrolAIController>(GetController());

	if (ControllerAI)
	{
		ControllerAI->SetDefendSelf(false);
	}
}
