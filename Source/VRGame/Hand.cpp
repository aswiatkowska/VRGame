
#include "Hand.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

	RightHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	RightHandSkeletal->SetupAttachment(Scene);
	RightHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

	LeftHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHand");
	LeftHandSkeletal->SetupAttachment(Scene);
	LeftHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

	GrabPoint = CreateDefaultSubobject<USceneComponent>("GrabPoint");
	GrabPoint->SetupAttachment(RightHandSkeletal);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionSphere->SetupAttachment(RightHandSkeletal);
	CollisionSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject), ECollisionResponse::ECR_Overlap);
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

	if (IsOverlaping)
	{
		for (auto i = overlapMap.CreateIterator(); i; ++i)
		{
			float distance = FVector::Dist(i.Key()->Location, CollisionSphere->GetComponentLocation());
			overlapMap.Emplace(i.Key(), distance);
		}
	}

}

void AHand::ObjectGrabRelease()
{
	if (!IsAnyObjectGrabbed())
	{
		float val1 = 0;
		for (auto i = overlapMap.CreateIterator(); i; ++i)
		{
			float val2 = overlapMap[i.Key()];
			if (val2 > val1)
			{
				val1 = val2;
			}
		}
		GrabbedObjectGrabbableComponent = (UGrabbableObjectComponent*)overlapMap.FindKey(val1);
		RightHandSkeletal->SetVisibility(false);
		GrabbedObjectGrabbableComponent->OnGrabDelegate.Broadcast();
		GrabbedActor = GrabbedObjectGrabbableComponent->GetOwner();
		GrabbedActor->AttachToComponent(GrabPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GrabPoint->SetRelativeLocation(GrabbedObjectGrabbableComponent->Location);
		GrabPoint->SetRelativeRotation(GrabbedObjectGrabbableComponent->Rotation);
	}
	else if (IsAnyObjectGrabbed())
	{
		RightHandSkeletal->SetVisibility(true);
		GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedObjectGrabbableComponent->OnReleaseDelegate.Broadcast();
		GrabbedObjectGrabbableComponent = nullptr;
		GrabbedActor = nullptr;
	}
}

void AHand::OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (IsAnyObjectGrabbed())
	{
		return;
	}
	
	IsOverlaping = true;
	DetectedGrabbable = OtherActor->FindComponentByClass<UGrabbableObjectComponent>();
	float distance = FVector::Dist(DetectedGrabbable->Location, CollisionSphere->GetComponentLocation());
	overlapMap.Add(DetectedGrabbable, distance);
	if (DetectedGrabbable == nullptr)
	{
		return;
	}
}

void AHand::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IsOverlaping = false;
	overlapMap.Remove(DetectedGrabbable);
}

bool AHand::IsAnyObjectGrabbed()
{
	return (GrabbedObjectGrabbableComponent != nullptr);
}

UGrabbableObjectComponent* AHand::GetGrabbedObject()
{
	return GrabbedObjectGrabbableComponent;
}

