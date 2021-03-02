
#include "MyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h" 
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "CustomChannels.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Scene->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(Scene);

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>("LeftMotionController");
	LeftMotionController->SetupAttachment(Scene);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>("RightMotionController");
	RightMotionController->SetupAttachment(Scene);
	RightMotionController->SetTrackingSource(EControllerHand::Right);

	GrabPoint = CreateDefaultSubobject<USceneComponent>("GrabPoint");
	GrabPoint->SetupAttachment(RightMotionController);

	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftHand");
	LeftHandMesh->SetupAttachment(LeftMotionController);

	RightHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	RightHandSkeletal->SetupAttachment(RightMotionController);
	RightHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionSphere->SetupAttachment(RightHandSkeletal);
	CollisionSphere->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::GrabbableObject), ECollisionResponse::ECR_Overlap);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	navmesh = dynamic_cast<ARecastNavMesh*>(UGameplayStatics::GetActorOfClass(GetWorld(), ARecastNavMesh::StaticClass()));
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	Scene->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnHandOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnHandOverlapEnd);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SwitchMotion)
	{
		TeleportLocation();
		
		if (TeleportLocation())
		{
			FVector Start = FVector(LeftMotionController->GetComponentLocation());
			FVector End = FVector(LeftMotionController->GetComponentLocation() + (LeftHandMesh->GetForwardVector() * 1000.0f));

			TArray<AActor*> ignored;
			ignored.Add(this);

			CanTeleport = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
				false, ignored, EDrawDebugTrace::ForOneFrame, hit, true);
		}
	}

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveBackwards", this, &AMyCharacter::MoveBackwards);
	PlayerInputComponent->BindAxis("MoveLeft", this, &AMyCharacter::MoveLeft);


	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMyCharacter::Teleport);
	PlayerInputComponent->BindAction("ChangeMotion", IE_Pressed, this, &AMyCharacter::ChangeMotion);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &AMyCharacter::TurnRight);
	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &AMyCharacter::TurnLeft);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMyCharacter::Shoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AMyCharacter::ShootingReleased);
	PlayerInputComponent->BindAction("GrabWeapon", IE_Pressed, this, &AMyCharacter::GrabWeapon);
	PlayerInputComponent->BindAction("ReleaseWeapon", IE_Released, this, &AMyCharacter::ReleaseWeapon);

}

void AMyCharacter::ChangeMotion()
{
	if (SwitchMotion)
	{
		SwitchMotion = false;
	}
	else
	{
		SwitchMotion = true;
	}
}

void AMyCharacter::MoveForward(float Value)
{
	if (SwitchMotion)
	{
		AddMovementInput(Camera->GetForwardVector(), Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if (SwitchMotion)
	{
		AddMovementInput(Camera->GetRightVector(), Value);
	}
}

void AMyCharacter::MoveBackwards(float Value)
{
	if (SwitchMotion)
	{
		AddMovementInput(Camera->GetForwardVector(), -Value);
	}
}

void AMyCharacter::MoveLeft(float Value)
{
	if (SwitchMotion)
	{
		AddMovementInput(Camera->GetRightVector(), -Value);
	}
}

void AMyCharacter::TurnRight()
{
	playerController->AddYawInput(30.0f / playerController->InputYawScale);
}

void AMyCharacter::TurnLeft()
{
	playerController->AddYawInput(-30.0f / playerController->InputYawScale);
}

bool AMyCharacter::TeleportLocation()
{
	FHitResult hitTeleport;
	const float TeleportRange = 1000.0f;
	const FVector Start = LeftMotionController->GetComponentLocation();
	const FVector End = (LeftMotionController->GetForwardVector() * TeleportRange) + Start;

	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(TeleportRange), false, this);

	if (GetWorld()->LineTraceSingleByChannel(hitTeleport, Start, End, ECC_Visibility, QueryParams))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AMyCharacter::Teleport()
{
	if (!SwitchMotion)
	{
		FNavLocation outnav;

		if (CanTeleport)
		{
			if (navmesh->ProjectPoint(hit.ImpactPoint, outnav, vector))
			{
				this->SetActorLocation(outnav);
			}
		}
	}
}

void AMyCharacter::GrabWeapon()
{
	if (CanGrab)
	{
		RightHandSkeletal->SetVisibility(false);
		Weapon->WeaponMesh->SetSimulatePhysics(false);
		Weapon->AttachToComponent(GrabPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GrabPoint->SetRelativeLocation(Weapon->Location);
		GrabPoint->SetRelativeRotation(Weapon->Rotation);
		WeaponGrabbed = true;
		CanGrab = false;
	}
}

void AMyCharacter::ReleaseWeapon()
{
	if (WeaponGrabbed)
	{
		RightHandSkeletal->SetVisibility(true);
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->WeaponMesh->SetSimulatePhysics(true);
		WeaponGrabbed = false;
	}
}

void AMyCharacter::Shoot()
{
	if (WeaponGrabbed)
	{
		Weapon->Shoot();
	}
}

void AMyCharacter::ShootingReleased()
{
	if (!WeaponGrabbed)
	{
		Weapon->ShootingReleased();
	}
}

void AMyCharacter::OnHandOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (WeaponGrabbed)
	{
		return;
	}
	Weapon = Cast<AWeapon>(OtherActor);
	CanGrab = (Weapon != nullptr);
}

void AMyCharacter::OnHandOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!WeaponGrabbed)
	{
		Weapon = nullptr;
		CanGrab = false;
	}
}
