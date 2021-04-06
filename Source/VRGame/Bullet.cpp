
#include "Bullet.h"
#include "CustomChannels.h"
#include "Components/StaticMeshComponent.h"

ABullet::ABullet()
{
 	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	RootComponent = BulletMesh;
	BulletMesh->SetSimulatePhysics(true);
	BulletMesh->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet));
	BulletMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::PatrolAI), ECR_Block);

	BulletArrowComponent = CreateDefaultSubobject<UArrowComponent>("BulletArrowComponent");
	BulletArrowComponent->SetupAttachment(BulletMesh);

}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	BulletMesh->AddImpulse(BulletArrowComponent->GetForwardVector() * BulletForce);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ABullet::OnDestroy, 5);
}

void ABullet::OnDestroy()
{
	Destroy();
}

