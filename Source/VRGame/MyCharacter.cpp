
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

	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftHand");
	LeftHandMesh->SetupAttachment(LeftMotionController);

	RightHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("RightHand");
	RightHandSkeletal->SetupAttachment(RightMotionController);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	CollisionSphere->SetupAttachment(RightHandSkeletal);
	CollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	navmesh = dynamic_cast<ARecastNavMesh*>(UGameplayStatics::GetActorOfClass(GetWorld(), ARecastNavMesh::StaticClass()));

	OnActorBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlap);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	Scene->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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

void AMyCharacter::OnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (Cast<AWeapon>(RightHandSkeletal) != nullptr)
	{
		CanGrab = true;
	}
	else
	{
		CanGrab = false;
	}
}


void AMyCharacter::GrabWeapon()
{
	if (CanGrab == true)
	{
		RightHandSkeletal->SetVisibility(false);
		Weapon->WeaponMesh->AttachToComponent(RightMotionController, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AMyCharacter::ReleaseWeapon()
{
	RightHandSkeletal->SetVisibility(true);
	Weapon->WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AMyCharacter::Shoot()
{
	Weapon->Shoot();
}