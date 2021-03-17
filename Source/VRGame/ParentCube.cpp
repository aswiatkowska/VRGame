
#include "ParentCube.h"
#include "Components/StaticMeshComponent.h"
#include "Bullet.h"

AParentCube::AParentCube()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
	CubeMesh->SetupAttachment(Root);
	CubeMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

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

