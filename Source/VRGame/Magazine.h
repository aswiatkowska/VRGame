
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObjectComponent.h"
#include "Weapon.h"
#include "Magazine.generated.h"

UCLASS()
class VRGAME_API AMagazine : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagazine();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MagazineMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGrabbableObjectComponent* GrabbableObjComp;

private:
	AWeapon* Weapon;

	virtual void BeginPlay() override;

	void DestroyMagazine();

	void AddMagazine();

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

};
