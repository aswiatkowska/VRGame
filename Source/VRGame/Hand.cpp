
#include "Hand.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GrabbableObjectComponent.h"
#include "CustomChannels.h"
#include "Engine/EngineTypes.h"

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Scene->SetupAttachment(GetRootComponent());

	RightHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	RightHandSkeletal->SetupAttachment(Scene);
	RightHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

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

}

void AHand::ObjectGrabRelease()
{
	if (CanGrab())
	{
		RightHandSkeletal->SetVisibility(false);
		GrabbedObjectGrabbableComponent = DetectedGrabbable;
		DetectedGrabbable = nullptr;
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

	DetectedGrabbable = OtherActor->FindComponentByClass<UGrabbableObjectComponent>();
	if (DetectedGrabbable == nullptr)
	{
		return;
	}
	
	if (DetectedGrabbable->GrabbableType == EGrabbableTypeEnum::EWeapon)
	{
		Weapon = Cast<AWeapon>(OtherActor);
	}
	else if (DetectedGrabbable->GrabbableType == EGrabbableTypeEnum::EMagazine)
	{
		Magazine = Cast<AMagazine>(OtherActor);
	}
}

void AHand::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsAnyObjectGrabbed())
	{
		DetectedGrabbable = nullptr;
	}
}

bool AHand::IsAnyObjectGrabbed()
{
	return (GrabbedObjectGrabbableComponent != nullptr);
}

bool AHand::CanGrab()
{
	return (DetectedGrabbable != nullptr);
}

UGrabbableObjectComponent* AHand::GetGrabbedObject()
{
	return GrabbedObjectGrabbableComponent;
}

