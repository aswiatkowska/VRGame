
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "GrabbableObjectComponent.h"
#include "Weapon.generated.h"

UCLASS()
class VRGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void Shoot();

	void ShootingReleased();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Barrel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGrabbableObjectComponent* GrabbableObjComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABullet> BulletSubclass;

	UPROPERTY(EditDefaultsOnly, Category = GrabbableObject)
	TEnumAsByte<EObjectToGrab::Type> GrabbableObjType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShootingSpree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Ammunition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagazineCapacity;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SwitchCoolDown();

	void AmmunitionCheck();

	int OwnedMagazinesCount = 1;

	bool IsOverlapped = false;

	bool IsPressed = false;

	bool cooldown;

};
