
#include "MyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h" 
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/PlayerController.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	Scene->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Scene);

	LeftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionController"));
	LeftMotionController->SetupAttachment(Scene);
	LeftMotionController->SetTrackingSource(EControllerHand::Left);

	RightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionController"));
	RightMotionController->SetupAttachment(Scene);
	RightMotionController->SetTrackingSource(EControllerHand::Right);

	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHand"));
	LeftHandMesh->SetupAttachment(LeftMotionController);

	RightHandSkeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHand"));
	RightHandSkeletal->SetupAttachment(RightMotionController);

	GunBarrel = CreateDefaultSubobject<USceneComponent>(TEXT("Barrel"));
	GunBarrel->SetupAttachment(RightHandSkeletal);

	navmesh = dynamic_cast<ARecastNavMesh*>(UGameplayStatics::GetActorOfClass(GetWorld(), ARecastNavMesh::StaticClass()));
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	Scene->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

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

	if (SwitchMotion)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward).bConsumeInput = false;
		PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	}
	else
	{
		PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMyCharacter::Teleport).bConsumeInput = false;
	}

	PlayerInputComponent->BindAction("ChangeMotion", IE_Pressed, this, &AMyCharacter::ChangeMotion);

	if (cooldown == false)
	{
		PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMyCharacter::Shoot);
	}

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

	SetupPlayerInputComponent(this->CreatePlayerInputComponent());
}

void AMyCharacter::MoveForward(float Value)
{
	//AddMovementInput(Camera->GetForwardVector(), Value);

	const FRotator Rotation = Camera->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AMyCharacter::MoveRight(float Value)
{
	//AddMovementInput(Camera->GetRightVector(), Value);

	const FRotator Rotation = Camera->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
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
	FNavLocation outnav;

	if (CanTeleport)
	{
		if (navmesh->ProjectPoint(hit.ImpactPoint, outnav, vector))
		{
			this->SetActorLocation(outnav);
		}
	}
}

void AMyCharacter::Shoot()
{
	FHitResult hitShoot;
	const float PistolRange = 2000.0f;
	const FVector Start = GunBarrel->GetComponentLocation();
	const FVector End = (GunBarrel->GetForwardVector() * PistolRange) + Start;

	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(PistolRange), false, this);

	if (GetWorld()->LineTraceSingleByChannel(hitShoot, Start, End, ECC_Visibility, QueryParams))
	{
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(hitShoot.ImpactNormal.Rotation(), hitShoot.ImpactPoint));
		}
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(GunBarrel->GetSocketRotation("Barrel"), GunBarrel->GetSocketLocation("Barrel")));

	cooldown = true;
	SetupPlayerInputComponent(this->CreatePlayerInputComponent());
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AMyCharacter::SwitchCoolDown, 0.5);
}

void AMyCharacter::SwitchCoolDown()
{
	cooldown = false;
	SetupPlayerInputComponent(this->CreatePlayerInputComponent());
}