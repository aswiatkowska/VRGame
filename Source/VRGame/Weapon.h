
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "Weapon.generated.h"

UCLASS()
class VRGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Shoot();

	void SwitchCoolDown();

	void DestroyBullet();

	bool cooldown;

	ABullet* BulletClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* Barrel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABullet> Bullet;

};
