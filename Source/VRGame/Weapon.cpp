
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CustomChannels.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	GrabbableObjComp = CreateDefaultSubobject<UGrabbableObjectComponent>("GrabbableObjComp");

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject), ECR_Ignore);
	WeaponMesh->SetSimulatePhysics(true);

	Barrel = CreateDefaultSubobject<USceneComponent>("Barrel");
	Barrel->SetupAttachment(WeaponMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Box");
	CollisionBox->SetupAttachment(WeaponMesh);
	CollisionBox->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	GrabbableObjComp->GrabbableType = EGrabbableTypeEnum::EWeapon;

	GrabbableObjComp->OnGrabDelegate.AddDynamic(this, &AWeapon::OnGrab);
	GrabbableObjComp->OnReleaseDelegate.AddDynamic(this, &AWeapon::OnRelease);

	Ammunition = MagazineCapacity;
}

void AWeapon::Shoot()
{
	if (cooldown || Ammunition <= 0)
	{
		return;
	}
	
	IsPressed = true;
	FHitResult hitShoot;
	const float PistolRange = 2000.0f;
	const FVector Start = Barrel->GetComponentLocation();
	const FVector End = (Barrel->GetForwardVector() * PistolRange) + Start;

	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(PistolRange), false, this);
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Barrel->GetComponentRotation(), Barrel->GetComponentLocation()));
	if (GetWorld()->LineTraceSingleByChannel(hitShoot, Start, End, ECC_Visibility, QueryParams))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(hitShoot.ImpactNormal.Rotation(), hitShoot.ImpactPoint));
	}

	GetWorld()->SpawnActor<AActor>(BulletSubclass, Start, Barrel->GetComponentRotation());
	
	if (!UnlimitedBullets)
	{
		Ammunition = Ammunition - 1;

		if (Ammunition <= 0)
		{
			AmmunitionCheck();
		}
	}

	cooldown = true;
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AWeapon::SwitchCoolDown, cooldownTime);
}

void AWeapon::ShootingReleased()
{
	IsPressed = false;
}

void AWeapon::SwitchCoolDown()
{
	cooldown = false;
	
	if (ShootingSpree && IsPressed)
	{
		Shoot();
	}
}

void AWeapon::AmmunitionCheck()
{
	MyCharacter = Cast<AMyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyCharacter::StaticClass()));
	if (MyCharacter->GetFromInventory(this->MagazineType))
	{
		this->Ammunition = this->MagazineCapacity;
	}
}

void AWeapon::OnGrab()
{
	WeaponMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::HandPhysical), ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetSimulatePhysics(false);
	if (Ammunition <= 0)
	{
		AmmunitionCheck();
	}
}

void AWeapon::OnRelease()
{
	WeaponMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::HandPhysical), ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetSimulatePhysics(true);
	ShootingReleased();
}

