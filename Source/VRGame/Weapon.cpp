
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	Barrel = CreateDefaultSubobject<USceneComponent>(TEXT("Barrel"));
	Barrel->SetupAttachment(WeaponMesh);

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
	FHitResult hitShoot;
	const float PistolRange = 2000.0f;
	const FVector Start = Barrel->GetComponentLocation();
	const FVector End = (Barrel->GetForwardVector() * PistolRange) + Start;

	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(PistolRange), false, this);

	if (cooldown == false)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Barrel->GetComponentRotation(), Barrel->GetComponentLocation()));

		if (GetWorld()->LineTraceSingleByChannel(hitShoot, Start, End, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(hitShoot.ImpactNormal.Rotation(), hitShoot.ImpactPoint));
		}

		GetWorld()->SpawnActor<AActor>(Bullet, Start, Barrel->GetComponentRotation());
	}

	cooldown = true;
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AWeapon::SwitchCoolDown, 0.5);
	DestroyBullet();
}

void AWeapon::SwitchCoolDown()
{
	cooldown = false;
}

void AWeapon::DestroyBullet()
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, BulletClass, &ABullet::OnDestroy, 3);
}

