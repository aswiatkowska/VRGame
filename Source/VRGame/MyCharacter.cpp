
#include "MyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h" 
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h" 
#include "Components/StaticMeshComponent.h"
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

	LeftHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(LeftMotionController);

	RightHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(RightMotionController);

	navmesh = dynamic_cast<ARecastNavMesh*>(UGameplayStatics::GetActorOfClass(GetWorld(), ARecastNavMesh::StaticClass()));
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	Scene->AddWorldOffset(-NewCameraOffset);

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (SwitchMotion)
	{
		PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMyCharacter::Teleport);
	}
	else
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	}

	PlayerInputComponent->BindAction("ChangeMotion", IE_Pressed, this, &AMyCharacter::ChangeMotion);

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
	AddMovementInput(Camera->GetForwardVector(), Value);
}

void AMyCharacter::Teleport()
{
	FHitResult hit;
	FVector vector = FVector(1000, 1000, 1000);
	FVector Start = FVector(LeftMotionController->GetComponentLocation());
	FVector End = FVector(LeftMotionController->GetComponentLocation() + (LeftHand->GetForwardVector() * 1000.0f));

	TArray<AActor*> ignored;
	ignored.Add(this);

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
		false, ignored, EDrawDebugTrace::Persistent, hit, true))
	{
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &AMyCharacter::ClearDebugLine, 0.5);

		FNavLocation outnav;

		if (navmesh->ProjectPoint(hit.ImpactPoint, outnav, vector))
		{
			this->SetActorLocation(outnav);
		}
	}
}

void AMyCharacter::ClearDebugLine()
{
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
}
