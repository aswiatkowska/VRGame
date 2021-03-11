
#include "MyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h" 
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "MotionControllerComponent.h"
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

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>("LeftMotionController");
	LeftMotionController->SetupAttachment(Scene);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>("RightMotionController");
	RightMotionController->SetupAttachment(Scene);
	RightMotionController->SetTrackingSource(EControllerHand::Right);

	LeftHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHand");
	LeftHandSkeletal->SetupAttachment(LeftMotionController);
	LeftHandSkeletal->SetCollisionObjectType((ECollisionChannel)(CustomCollisionChannelsEnum::Hand));

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(Scene);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	navmesh = dynamic_cast<ARecastNavMesh*>(UGameplayStatics::GetActorOfClass(GetWorld(), ARecastNavMesh::StaticClass()));
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	Scene->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	Hand = GetWorld()->SpawnActor<AHand>(HandClass, FVector::ZeroVector, FRotator::ZeroRotator);
	Hand->AttachToComponent(RightMotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	Magazine = Cast<AMagazine>(UGameplayStatics::GetActorOfClass(GetWorld(), AMagazine::StaticClass()));
	Magazine->OnAddToInvDelegate.AddDynamic(this, &AMyCharacter::ObjectToAdd);

	WeaponClass = Cast<AWeapon>(UGameplayStatics::GetActorOfClass(GetWorld(), AWeapon::StaticClass()));
	WeaponClass->OnRemoveFromInvDelegate.AddDynamic(this, &AMyCharacter::ObjectToRemove);
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
			FVector End = FVector(LeftMotionController->GetComponentLocation() + (LeftHandSkeletal->GetForwardVector() * 1000.0f));

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
	PlayerInputComponent->BindAction("GrabWeapon", IE_Pressed, this, &AMyCharacter::ObjectGrabRelease);
	PlayerInputComponent->BindAction("ReleaseWeapon", IE_Released, this, &AMyCharacter::ObjectGrabRelease);

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

void AMyCharacter::ObjectGrabRelease()
{
	Hand->ObjectGrabRelease();
}

void AMyCharacter::Shoot()
{
	if (Hand->GetGrabbedObject() != nullptr)
	{
		if (Hand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			Weapon = Cast<AWeapon>(Hand->GetGrabbedObject()->GetOwner());
			Weapon->Shoot();
		}
	}
}

void AMyCharacter::ShootingReleased()
{
	if (Hand->GetGrabbedObject() != nullptr)
	{
		if (Hand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			Weapon = Cast<AWeapon>(Hand->GetGrabbedObject()->GetOwner());
			Weapon->ShootingReleased();
		}
	}
}

void AMyCharacter::ObjectToAdd()
{
	AMagazine* GrabbedMagazine = (AMagazine*)Hand->GetGrabbedObject()->GetOwner();
	EInventoryObjectTypes Type = GrabbedMagazine->InvObjectType;

	InvMap->AddObject(Type);

}

void AMyCharacter::ObjectToRemove()
{
	AWeapon* GrabbedWeapon = (AWeapon*)Hand->GetGrabbedObject()->GetOwner();
	EWeaponTypeEnum WeaponType = GrabbedWeapon->WeaponType;

	if (WeaponType == EWeaponTypeEnum::EGun)
	{
		if (InvMap->IsInInventory(EInventoryObjectTypes::Magazine_pistol))
		{
			InvMap->RemoveObject(EInventoryObjectTypes::Magazine_pistol);
			GrabbedWeapon->Ammunition = GrabbedWeapon->MagazineCapacity;
		}
	}
	else if (WeaponType == EWeaponTypeEnum::ERifle)
	{
		if (InvMap->IsInInventory(EInventoryObjectTypes::Magazine_rifle))
		{
			InvMap->RemoveObject(EInventoryObjectTypes::Magazine_rifle);
			GrabbedWeapon->Ammunition = GrabbedWeapon->MagazineCapacity;
		}
	}
}
