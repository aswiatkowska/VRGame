
#include "MyCharacter.h"
#include "Weapon.h"
#include "Bullet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
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

	DamageScreen = CreateDefaultSubobject<UStaticMeshComponent>("DamageScreen");
	DamageScreen->SetupAttachment(Camera);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::Bullet), ECollisionResponse::ECR_Overlap);

	DamageScreen->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::HandPhysical), ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel((ECollisionChannel)(CustomCollisionChannelsEnum::HandPhysical), ECollisionResponse::ECR_Ignore);

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
	RightHand->AttachToComponent(RightMotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	LeftHand = GetWorld()->SpawnActor<AHand>(HandClass, FVector::ZeroVector, FRotator::ZeroRotator);
	LeftHand->AttachToComponent(LeftMotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	LeftHand->HandSkeletal->SetRelativeScale3D(FVector(1.0f, -1.0f, 1.0f));
	
	LeftHand->SetupHand(EHandEnum::ELeft, RightHand);
	RightHand->SetupHand(EHandEnum::ERight, LeftHand);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnBulletOverlapBegin);

	DamageDynamicMaterial = DamageScreen->CreateAndSetMaterialInstanceDynamic(0);
	DamageDynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0);
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

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyCharacter::CrouchPlayer);

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
	QueryParams.AddIgnoredActor(RightHand);
	QueryParams.AddIgnoredActor(LeftHand);

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
}

void AMyCharacter::ObjectReleaseRight()
{
	RightHand->ObjectRelease();
}

void AMyCharacter::ObjectGrabLeft()
{
	LeftHand->ObjectGrab();
}

void AMyCharacter::ObjectReleaseLeft()
{
	LeftHand->ObjectRelease();
}

void AMyCharacter::ShootRight()
{
	if (RightHand->GetGrabbedObject() != nullptr)
	{
		if (RightHand->GetGrabbedObject()->GrabbableType == EGrabbableTypeEnum::EWeapon)
		{
			AWeapon* Weapon = Cast<AWeapon>(RightHand->GetGrabbedObject()->GetOwner());
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
			AWeapon* Weapon = Cast<AWeapon>(LeftHand->GetGrabbedObject()->GetOwner());
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
			AWeapon* Weapon = Cast<AWeapon>(RightHand->GetGrabbedObject()->GetOwner());
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
			AWeapon* Weapon = Cast<AWeapon>(LeftHand->GetGrabbedObject()->GetOwner());
			Weapon->ShootingReleased();
		}
	}
}

void AMyCharacter::CrouchPlayer()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
		Crouch();
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

void AMyCharacter::OnBulletOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<ABullet>(OtherActor) != nullptr)
	{
		ABullet* Bullet = Cast<ABullet>(OtherActor);
		NumberOfLifes = NumberOfLifes - Bullet->BulletImpact;

		if (NumberOfLifes <= 0)
		{
			DamageDynamicMaterial->SetScalarParameterValue(FName("Opacity"), 1);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &AMyCharacter::RemoveDamageMaterial, 1);
		}
		else
		{
			DamageDynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0.5);
			FTimerHandle timerHandle;
			GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AMyCharacter::RemoveDamageMaterial, 0.5);
		}
	}
}

void AMyCharacter::RemoveDamageMaterial()
{
	DamageDynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0);
	if (NumberOfLifes <= 0)
	{
		PlayerDeath();
	}
}

void AMyCharacter::PlayerDeath()
{
	DamageDynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor(0, 0, 0, 1));
	DamageDynamicMaterial->SetScalarParameterValue(FName("Opacity"), 1);
	RightHand->ObjectRelease();
	LeftHand->ObjectRelease();
	GetCharacterMovement()->DisableMovement();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	this->DisableInput(PlayerController);
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AMyCharacter::RestartGame, 1);
}

void AMyCharacter::RestartGame()
{
	UGameplayStatics::OpenLevel(this, "NewLevel", false);
}
