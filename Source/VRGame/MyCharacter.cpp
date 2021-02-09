
#include "MyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h" 
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
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

	Scene->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (SwitchMotion)
	{
		PlayerInputComponent->BindAction("DrawDebugLine", IE_Pressed, this, &AMyCharacter::DrawDebugLine);
		PlayerInputComponent->BindAction("Teleport", IE_Released, this, &AMyCharacter::Teleport);
	}
	else
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	}

	PlayerInputComponent->BindAction("ChangeMotion", IE_Pressed, this, &AMyCharacter::ChangeMotion);
	PlayerInputComponent->BindAction("MoveControllerRight", IE_Pressed, this, &AMyCharacter::MoveControllerRight);
	PlayerInputComponent->BindAction("MoveControllerLeft", IE_Pressed, this, &AMyCharacter::MoveControllerLeft);

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

void AMyCharacter::MoveRight(float Value)
{
	AddMovementInput(Camera->GetRightVector(), Value);
}

void AMyCharacter::MoveControllerRight()
{
	
}

void AMyCharacter::MoveControllerLeft()
{

}

void AMyCharacter::DrawDebugLine()
{
	FVector Start = FVector(LeftMotionController->GetComponentLocation());
	FVector End = FVector(LeftMotionController->GetComponentLocation() + (LeftHand->GetForwardVector() * 1000.0f));

	TArray<AActor*> ignored;
	ignored.Add(this);

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic), 
		false, ignored, EDrawDebugTrace::Persistent, hit, true);
}

void AMyCharacter::Teleport()
{
	ClearDebugLine();
	FNavLocation outnav;

	if (navmesh->ProjectPoint(hit.ImpactPoint, outnav, vector))
	{
			this->SetActorLocation(outnav);
	}
}

void AMyCharacter::ClearDebugLine()
{
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
}
