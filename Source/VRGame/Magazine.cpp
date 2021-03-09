
#include "Magazine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GrabbableObjectComponent.h"
#include "CustomChannels.h"

AMagazine::AMagazine()
{
	PrimaryActorTick.bCanEverTick = true;

	GrabbableObjComp = CreateDefaultSubobject<UGrabbableObjectComponent>("GrabbableObjComp");

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>("MagazineMesh");

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Box");
	CollisionBox->SetupAttachment(MagazineMesh);
	CollisionBox->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);
}

void AMagazine::DestroyMagazine()
{
	Destroy();
}

