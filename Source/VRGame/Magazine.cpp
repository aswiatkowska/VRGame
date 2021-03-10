
#include "Magazine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CustomChannels.h"

AMagazine::AMagazine()
{
	PrimaryActorTick.bCanEverTick = true;

	GrabbableObjComp = CreateDefaultSubobject<UGrabbableObjectComponent>("GrabbableObjComp");

	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>("MagazineMesh");
	MagazineMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Ignore);
	MagazineMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECR_Ignore);
	MagazineMesh->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject), ECR_Ignore);
	MagazineMesh->SetSimulatePhysics(true);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Box");
	CollisionBox->SetupAttachment(MagazineMesh);
	CollisionBox->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject));
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Hand), ECollisionResponse::ECR_Overlap);
}

void AMagazine::BeginPlay()
{
	Super::BeginPlay();

	GrabbableObjComp->GrabbableType = EGrabbableTypeEnum::EMagazine;

	GrabbableObjComp->OnGrabDelegate.AddDynamic(this, &AMagazine::OnGrab);
	GrabbableObjComp->OnReleaseDelegate.AddDynamic(this, &AMagazine::OnRelease);

	Weapon = Cast<AWeapon>(UGameplayStatics::GetActorOfClass(GetWorld(), AWeapon::StaticClass()));
}

void AMagazine::DestroyMagazine()
{
	Destroy();
}

void AMagazine::AddMagazine()
{
	Weapon->OwnedMagazinesCount++;
}

void AMagazine::OnGrab()
{
	MagazineMesh->SetSimulatePhysics(false);
	AddMagazine();
}

void AMagazine::OnRelease()
{
	MagazineMesh->SetSimulatePhysics(true);
	DestroyMagazine();
}

