
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "InventoryObjectTypes.h"
#include "MyCharacter.h"
#include "GrabbableObjectComponent.h"
#include "Weapon.generated.h"

UENUM()
enum EWeaponTypeEnum
{
	EGun,
	ERifle,
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWeaponTypeEnum> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EInventoryObjectTypes> MagazineType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float cooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShootingSpree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagazineCapacity;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	int Ammunition;

	void SwitchCoolDown();

	void AmmunitionCheck();

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

	bool IsPressed = false;

	bool cooldown;

	AMyCharacter* MyCharacter;

};
