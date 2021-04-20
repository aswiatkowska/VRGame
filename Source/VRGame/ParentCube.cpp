
#include "ParentCube.h"
#include "CustomChannels.h"
#include "Components/StaticMeshComponent.h"
#include "Bullet.h"

AParentCube::AParentCube()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
	CubeMesh->SetupAttachment(Root);
	CubeMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Overlap);
	CubeMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::HandPhysical), ECollisionResponse::ECR_Block);

	OnActorBeginOverlap.AddDynamic(this, &AParentCube::OnOverlap);

}

void AParentCube::OnDestroy()
{
	Destroy();
}

void AParentCube::OnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Cast<ABullet>(OtherActor) != nullptr)
	{
		OnDestroy();
	}
}

