
#include "Bullet.h"
#include "Components/StaticMeshComponent.h"

ABullet::ABullet()
{
 	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	RootComponent = BulletMesh;
	BulletMesh->SetSimulatePhysics(true);

	BulletArrowComponent = CreateDefaultSubobject<UArrowComponent>("BulletArrowComponent");
	BulletArrowComponent->SetupAttachment(BulletMesh);

}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	BulletMesh->AddImpulse(BulletArrowComponent->GetForwardVector() * BulletForce);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABullet::OnDestroy, 3);
}

void ABullet::OnDestroy()
{
	Destroy();
}

