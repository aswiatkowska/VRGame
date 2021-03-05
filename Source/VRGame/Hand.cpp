
#include "Hand.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "CustomChannels.h"

AHand::AHand()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Scene->SetupAttachment(GetRootComponent());

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>("LeftMotionController");
	LeftMotionController->SetupAttachment(Scene);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>("RightMotionController");
	RightMotionController->SetupAttachment(Scene);
	RightMotionController->SetTrackingSource(EControllerHand::Right);

	GrabPoint = CreateDefaultSubobject<USceneComponent>("GrabPoint");
	GrabPoint->SetupAttachment(RightMotionController);

	LeftHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHand");
	LeftHandSkeletal->SetupAttachment(LeftMotionController);
	LeftHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

	RightHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	RightHandSkeletal->SetupAttachment(RightMotionController);
	RightHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

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

void AHand::WeaponGrabRelease()
{
	if (CanGrab)
	{
		RightHandSkeletal->SetVisibility(false);
		Weapon->WeaponMesh->SetSimulatePhysics(false);
		Weapon->AttachToComponent(GrabPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GrabPoint->SetRelativeLocation(Weapon->Location);
		GrabPoint->SetRelativeRotation(Weapon->Rotation);
		WeaponGrabbed = true;
		CanGrab = false;
	}
	else if (WeaponGrabbed)
	{
		RightHandSkeletal->SetVisibility(true);
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->WeaponMesh->SetSimulatePhysics(true);
		WeaponGrabbed = false;
		Weapon->ShootingReleased();
	}
}

void AHand::Shoot()
{
	if (WeaponGrabbed)
	{
		Weapon->Shoot();
	}
}

void AHand::ShootingReleased()
{
	if (WeaponGrabbed)
	{
		Weapon->ShootingReleased();
	}
}

void AHand::OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (WeaponGrabbed)
	{
		return;
	}
	Weapon = Cast<AWeapon>(OtherActor);
	CanGrab = (Weapon != nullptr);
}

void AHand::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!WeaponGrabbed)
	{
		Weapon = nullptr;
		CanGrab = false;
	}
}

