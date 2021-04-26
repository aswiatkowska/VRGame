
#include "Hand.h"
#include "PatrolAI.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/SphereComponent.h"
#include "GrabbableObjectComponent.h"
#include "CustomChannels.h"
#include "Math/Vector.h"
#include "Engine/EngineTypes.h"

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Scene->SetupAttachment(GetRootComponent());

	HandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	HandSkeletal->SetupAttachment(Scene);
	HandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::HandPhysical));

	GrabPoint = CreateDefaultSubobject<USceneComponent>("GrabPoint");
	GrabPoint->SetupAttachment(HandSkeletal);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionSphere->SetupAttachment(HandSkeletal);
	CollisionSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject), ECollisionResponse::ECR_Overlap);

	PhysicalHand = CreateDefaultSubobject<UStaticMeshComponent>("PhysicalHand");
	PhysicalHand->SetupAttachment(Scene);
	PhysicalHand->SetCanEverAffectNavigation(false);
	PhysicalHand->SetVisibility(false);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraint");
	PhysicsConstraint->SetupAttachment(PhysicalHand);

	PhysicsConstraintGrab = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraintGrab");
	PhysicsConstraintGrab->SetupAttachment(HandSkeletal);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>("PhysicsHandle");

	HandSkeletal->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AHand::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHand::OnHandOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AHand::OnHandOverlapEnd);

}

void AHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (overlapMap.Num() > 0)
	{
		float distance;
		for (auto i = overlapMap.CreateIterator(); i; ++i)
		{
			if (i.Key()->CollisionComponent != nullptr)
			{
				distance = FVector::Dist(i.Key()->CollisionComponent->GetComponentLocation(), CollisionSphere->GetComponentLocation());
			}
			else
			{
				distance = FVector::Dist(i.Key()->GetOwner()->GetActorLocation(), CollisionSphere->GetComponentLocation());
			}

			overlapMap.Emplace(i.Key(), distance);
		}
	}

}

void AHand::ObjectGrab()
{
	if (!IsAnyObjectGrabbed() && overlapMap.Num() > 0)
	{
		UGrabbableObjectComponent* nearestObject = nullptr;
		float val1 = 1000;
		for (auto i = overlapMap.CreateIterator(); i; ++i)
		{
			float val2 = overlapMap[i.Key()];
			if (val2 < val1)
			{
				val1 = val2;
				nearestObject = i.Key();
			}
		}
		if (nearestObject->IsGrabbed)
		{
			nearestObject->OnHandChangedDelegate.Broadcast();
			OtherHand->ForceRelease();
		}
		nearestObject->IsGrabbed = true;
		GrabbedObjectGrabbableComponent = nearestObject;
		GrabbedActor = GrabbedObjectGrabbableComponent->GetOwner();

		if (GrabbedObjectGrabbableComponent->CanBeGrabbed)
		{
			if (GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::ERagdollHand || GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::ERagdollLeg)
			{
				APatrolAI* GrabbedPatrolAI = Cast<APatrolAI>(GrabbedActor);
				HandSkeletal->SetVisibility(false);
				PhysicsConstraintGrab->SetConstrainedComponents(GrabbedPatrolAI->GetMesh(), GrabbedObjectGrabbableComponent->BoneName,
					HandSkeletal, "hand_r");
			}
			else
			{
				if (GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::EWeapon)
				{
					AWeapon* Weapon = Cast<AWeapon>(GrabbedActor);
					Weapon->IsHeldByPlayer = true;
				}
				GrabbedObjectGrabbableComponent->OnGrabDelegate.Broadcast();
				HandSkeletal->SetVisibility(false);
				GrabbedActor->AttachToComponent(GrabPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
		}
		else
		{
			return;
		}

		if (HandType == EHandEnum::ERight)
		{
			GrabPoint->SetRelativeLocation(GrabbedObjectGrabbableComponent->RightLocation);
			GrabPoint->SetRelativeRotation(GrabbedObjectGrabbableComponent->RightRotation);
		}
		else if (HandType == EHandEnum::ELeft)
		{
			GrabPoint->SetRelativeLocation(GrabbedObjectGrabbableComponent->LeftLocation);
			GrabPoint->SetRelativeRotation(GrabbedObjectGrabbableComponent->LeftRotation);
		}
	}
}

void AHand::ObjectRelease()
{
	if (IsAnyObjectGrabbed())
	{
		if (GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::ERagdollHand || GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::ERagdollLeg)
		{
			PhysicsConstraintGrab->TermComponentConstraint();
		}
		else
		{
			GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		GrabbedActor->FindComponentByClass<UGrabbableObjectComponent>()->IsGrabbed = false;
		HandSkeletal->SetVisibility(true);
		GrabbedObjectGrabbableComponent->OnReleaseDelegate.Broadcast();
		if (GrabbedActor->IsActorBeingDestroyed())
		{
			overlapMap.Remove(GrabbedObjectGrabbableComponent);
		}
		GrabbedObjectGrabbableComponent = nullptr;
		GrabbedActor = nullptr;
	}
}

void AHand::ForceRelease()
{
	if (GrabbedActor != nullptr)
	{
		GrabbedActor->FindComponentByClass<UGrabbableObjectComponent>()->IsGrabbed = false;
		HandSkeletal->SetVisibility(true);
		if (GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::ERagdollHand || GrabbedObjectGrabbableComponent->GrabbableType == EGrabbableTypeEnum::ERagdollLeg)
		{
			PhysicsConstraintGrab->TermComponentConstraint();
		}
		else
		{
			GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		GrabbedObjectGrabbableComponent = nullptr;
		GrabbedActor = nullptr;
	}
}

void AHand::OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (IsAnyObjectGrabbed() || OtherActor->IsActorBeingDestroyed())
	{
		return;
	}

	TArray<UActorComponent*> GrabbableCompArray = OtherActor->GetComponentsByClass(UGrabbableObjectComponent::StaticClass());
	float compDistance;
	float shortestDistance = 1000;
	UGrabbableObjectComponent* CurrentComp;
	UGrabbableObjectComponent* DetectedGrabbable = nullptr;
	
	if (GrabbableCompArray.Num() > 1)
	{
		for (int i = 0; i < GrabbableCompArray.Num(); ++i)
		{
			CurrentComp = (UGrabbableObjectComponent*)GrabbableCompArray[i];
			compDistance = FVector::Dist(CurrentComp->CollisionComponent->GetComponentLocation(), CollisionSphere->GetComponentLocation());

			if (compDistance < shortestDistance)
			{
				shortestDistance = compDistance;
				DetectedGrabbable = CurrentComp;
			}
		}
	}
	else
	{
		DetectedGrabbable = OtherActor->FindComponentByClass<UGrabbableObjectComponent>();
	}

	if (DetectedGrabbable == nullptr)
	{
		return;
	}
	float distance = FVector::Dist(OtherActor->GetActorLocation(), CollisionSphere->GetComponentLocation());
	overlapMap.Add(DetectedGrabbable, distance);

	
}

void AHand::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<UActorComponent*> CompArray = OtherActor->GetComponentsByClass(UGrabbableObjectComponent::StaticClass());
	if (CompArray.Num() > 1)
	{
		float compDistance;
		float shortestDistance = 1000;
		UGrabbableObjectComponent* CurrentComp;
		UGrabbableObjectComponent* NearestComp = nullptr;

		for (int i = 0; i < CompArray.Num(); ++i)
		{
			CurrentComp = Cast<UGrabbableObjectComponent>(CompArray[i]);
			compDistance = FVector::Dist(CurrentComp->CollisionComponent->GetComponentLocation(), CollisionSphere->GetComponentLocation());

			if (compDistance < shortestDistance)
			{
				shortestDistance = compDistance;
				NearestComp = CurrentComp;
			}
		}

		overlapMap.Remove(NearestComp);
	}
	else
	{
		overlapMap.Remove(OtherActor->FindComponentByClass<UGrabbableObjectComponent>());
	}
}

bool AHand::IsAnyObjectGrabbed()
{
	return (GrabbedObjectGrabbableComponent != nullptr);
}

UGrabbableObjectComponent* AHand::GetGrabbedObject()
{
	return GrabbedObjectGrabbableComponent;
}

void AHand::SetupHand(EHandEnum RightOrLeft, AHand* OppositeHand)
{
	HandType = RightOrLeft;
	OtherHand = OppositeHand;
}

