
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

	InvMap = GetWorld()->SpawnActor<AInventory>(AInventory::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	RightHand = GetWorld()->SpawnActor<AHand>(HandClass, FVector::ZeroVector, FRotator::ZeroRotator);
	RightHand->HandSkeletal->AttachToComponent(RightMotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	LeftHand = GetWorld()->SpawnActor<AHand>(HandClass, FVector::ZeroVector, FRotator::ZeroRotator);
	LeftHand->HandSkeletal->AttachToComponent(LeftMotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	LeftHand->HandSkeletal->SetRelativeScale3D(FVector(1.0f, -1.0f, 1.0f));
	
	LeftHand->SetupHand(EHandEnum::ELeft, RightHand);
	RightHand->SetupHand(EHandEnum::ERight, LeftHand);
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
			FVector End = FVector(LeftMotionController->GetComponentLocation() + (LeftMotionController->GetForwardVector() * 1000.0f));

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

	PlayerInputComponent->BindAction("ShootRight", IE_Pressed, this, &AMyCharacter::ShootRight);
	PlayerInputComponent->BindAction("ShootRight", IE_Released, this, &AMyCharacter::ShootingReleasedRight);
	PlayerInputComponent->BindAction("ShootLeft", IE_Pressed, this, &AMyCharacter::ShootLeft);
	PlayerInputComponent->BindAction("ShootLeft", IE_Released, this, &AMyCharacter::ShootingReleasedLeft);

	PlayerInputComponent->BindAction("GrabReleaseRight", IE_Pressed, this, &AMyCharacter::ObjectGrabRight);
	PlayerInputComponent->BindAction("GrabReleaseRight", IE_Released, this, &AMyCharacter::ObjectReleaseRight);
	PlayerInputComponent->BindAction("GrabReleaseLeft", IE_Pressed, this, &AMyCharacter::ObjectGrabLeft);
	PlayerInputComponent->BindAction("GrabReleaseLeft", IE_Released, this, &AMyCharacter::ObjectReleaseLeft);

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

void AMyCharacter::ObjectGrabRight()
{
	RightHand->ObjectGrab();
	RightHand->RightHandGrabbing = true;
}

void AMyCharacter::ObjectReleaseRight()
{
	RightHand->ObjectRelease();
	RightHand->RightHandGrabbing = false;
}

void AMyCharacter::ObjectGrabLeft()
{
	LeftHand->ObjectGrab();
	LeftHand->LeftHandGrabbing = true;
}

void AMyCharacter::ObjectReleaseLeft()
{
	LeftHand->ObjectRelease();
	LeftHand->LeftHandGrabbing = false;
}

void AMyCharacter::ShootRight()
{
	if (RightHand->GetGrabbedObject() != nullptr)
	{
		if (RightHand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			Weapon = Cast<AWeapon>(RightHand->GetGrabbedObject()->GetOwner());
			PatrolAI->ChangeCurrentWeaponType(Weapon->WeaponType);
			Weapon->Shoot();
		}
	}
}

void AMyCharacter::ShootLeft()
{
	if (LeftHand->GetGrabbedObject() != nullptr)
	{
		if (LeftHand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			Weapon = Cast<AWeapon>(LeftHand->GetGrabbedObject()->GetOwner());
			PatrolAI->ChangeCurrentWeaponType(Weapon->WeaponType);
			Weapon->Shoot();
		}
	}
}

void AMyCharacter::ShootingReleasedRight()
{
	if (RightHand->GetGrabbedObject() != nullptr)
	{
		if (RightHand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			Weapon = Cast<AWeapon>(RightHand->GetGrabbedObject()->GetOwner());
			Weapon->ShootingReleased();
		}
	}
}

void AMyCharacter::ShootingReleasedLeft()
{
	if (LeftHand->GetGrabbedObject() != nullptr)
	{
		if (LeftHand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			Weapon = Cast<AWeapon>(LeftHand->GetGrabbedObject()->GetOwner());
			Weapon->ShootingReleased();
		}
	}
}

bool AMyCharacter::GetFromInventory(EInventoryObjectTypes Type)
{
	if (InvMap->IsInInventory(Type))
	{
		InvMap->RemoveObject(Type);
		return true;
	}
	else
	{
		return false;
	}
}

void AMyCharacter::AddToInventory(EInventoryObjectTypes Type)
{
	InvMap->AddObject(Type);
}
