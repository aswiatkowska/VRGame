
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "MyCharacter.h"
#include "InventoryObjectTypes.h"
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

	bool IsHeldByPlayer = false;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UnlimitedBullets = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	int Ammunition;

	void SwitchCoolDown();

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

	UFUNCTION()
	void OnMagazineOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	bool IsPressed = false;

	bool cooldown;

	AMyCharacter* MyCharacter;
};
