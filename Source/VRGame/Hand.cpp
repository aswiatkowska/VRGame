
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
	if (CanGrab && Weapon != nullptr)
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
	else if (CanGrab && Magazine != nullptr)
	{
		RightHandSkeletal->SetVisibility(false);
		Magazine->MagazineMesh->SetSimulatePhysics(false);
		Magazine->AttachToComponent(GrabPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GrabPoint->SetRelativeLocation(Magazine->Location);
		GrabPoint->SetRelativeRotation(Magazine->Rotation);
		Magazine->AddMagazine();
		MagazineGrabbed = true;
		CanGrab = false;
	}
	else if (MagazineGrabbed)
	{
		RightHandSkeletal->SetVisibility(true);
		Magazine->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Magazine->MagazineMesh->SetSimulatePhysics(true);
		MagazineGrabbed = false;
		Magazine->DestroyMagazine();
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
	if (WeaponGrabbed || MagazineGrabbed)
	{
		return;
	}

	UGrabbableObjectComponent* grabbableComponent = OtherActor->FindComponentByClass<UGrabbableObjectComponent>();
	if (grabbableComponent == nullptr)
	{
		return;
	}
	
	if (grabbableComponent->GrabbableType == EGrabbableTypeEnum::EWeapon)
	{
		Weapon = Cast<AWeapon>(OtherActor);
	}
	else if (grabbableComponent->GrabbableType == EGrabbableTypeEnum::EMagazine)
	{
		Magazine = Cast<AMagazine>(OtherActor);
	}
	CanGrab = true;
}

void AHand::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!WeaponGrabbed && !MagazineGrabbed)
	{
		Weapon = nullptr;
		Magazine = nullptr;
		CanGrab = false;
	}
}

