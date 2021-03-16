
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

	HandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	HandSkeletal->SetupAttachment(Scene);
	HandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

	GrabPoint = CreateDefaultSubobject<USceneComponent>("GrabPoint");
	GrabPoint->SetupAttachment(HandSkeletal);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionSphere->SetupAttachment(HandSkeletal);
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

	if (overlapMap.Num() > 0)
	{
		for (auto i = overlapMap.CreateIterator(); i; ++i)
		{
			float distance = FVector::Dist(i.Key()->GetOwner()->GetActorLocation(), CollisionSphere->GetComponentLocation());
			overlapMap.Emplace(i.Key(), distance);
		}
	}

}

void AHand::ObjectGrabRelease()
{
	if (!IsAnyObjectGrabbed() && (overlapMap.Num() > 0))
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
		GrabbedObjectGrabbableComponent = nearestObject;
		HandSkeletal->SetVisibility(false);
		GrabbedObjectGrabbableComponent->OnGrabDelegate.Broadcast();
		GrabbedActor = GrabbedObjectGrabbableComponent->GetOwner();
		GrabbedActor->AttachToComponent(GrabPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
	else if (IsAnyObjectGrabbed())
	{
		HandSkeletal->SetVisibility(true);
		GrabbedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedObjectGrabbableComponent->OnReleaseDelegate.Broadcast();
		if (GrabbedActor->IsActorBeingDestroyed())
		{
			overlapMap.Remove(GrabbedObjectGrabbableComponent);
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
	
	UGrabbableObjectComponent* DetectedGrabbable = OtherActor->FindComponentByClass<UGrabbableObjectComponent>();
	if (DetectedGrabbable == nullptr)
	{
		return;
	}
	//UE_LOG(LogTemp, Log, TEXT("Overlap begin z %s"), *(OtherActor->GetName()))
	float distance = FVector::Dist(OtherActor->GetActorLocation(), CollisionSphere->GetComponentLocation());
	overlapMap.Add(DetectedGrabbable, distance);

	
}

void AHand::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	overlapMap.Remove(OtherActor->FindComponentByClass<UGrabbableObjectComponent>());
	//UE_LOG(LogTemp, Log, TEXT("Overlap end z %s"), *(OtherActor->GetName()))
}

bool AHand::IsAnyObjectGrabbed()
{
	return (GrabbedObjectGrabbableComponent != nullptr);
}

UGrabbableObjectComponent* AHand::GetGrabbedObject()
{
	return GrabbedObjectGrabbableComponent;
}

