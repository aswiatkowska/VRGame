
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CustomChannels.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetSimulatePhysics(true);

	Barrel = CreateDefaultSubobject<USceneComponent>("Barrel");
	Barrel->SetupAttachment(WeaponMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Box");
	CollisionBox->SetupAttachment(WeaponMesh);
	CollisionBox->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);

	Location = WeaponMesh->GetComponentLocation();
	Rotation = WeaponMesh->GetComponentRotation();

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::Shoot()
{
	IsPressed = true;
	FHitResult hitShoot;
	const float PistolRange = 2000.0f;
	const FVector Start = Barrel->GetComponentLocation();
	const FVector End = (Barrel->GetForwardVector() * PistolRange) + Start;

	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(PistolRange), false, this);

	if (cooldown == false && Ammunition > 0)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Barrel->GetComponentRotation(), Barrel->GetComponentLocation()));

		if (GetWorld()->LineTraceSingleByChannel(hitShoot, Start, End, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(hitShoot.ImpactNormal.Rotation(), hitShoot.ImpactPoint));
		}

		GetWorld()->SpawnActor<AActor>(BulletSubclass, Start, Barrel->GetComponentRotation());
	}

	Ammunition = Ammunition - 1;
	cooldown = true;
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AWeapon::SwitchCoolDown, 0.5);
}

void AWeapon::ShootingReleased()
{
	IsPressed = false;
}

void AWeapon::SwitchCoolDown()
{
	cooldown = false;
	
	if (ShootingSpree() && IsPressed)
	{
		Shoot();
	}
}

bool AWeapon::ShootingSpree()
{
	return true;
}

