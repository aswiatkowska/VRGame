
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "GrabbableObjectComponent.h"
#include "Weapon.generated.h"

UENUM()
enum EWeaponTypeEnum
{
	EGun,
	ERifle,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRemoveFromInvDelegate);

UCLASS()
class VRGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	
	virtual void Shoot();

	void ShootingReleased();

	int OwnedMagazinesCount = 1;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWeaponTypeEnum> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShootingSpree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Ammunition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagazineCapacity;

	FRemoveFromInvDelegate OnRemoveFromInvDelegate;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SwitchCoolDown();

	void AmmunitionCheck();

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

	bool IsOverlapped = false;

	bool IsPressed = false;

	bool cooldown;

};
