
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObjectComponent.h"
#include "Inventory.h"
#include "Magazine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAddToInvDelegate);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EInventoryObjectTypes> InvObjectType;

	FAddToInvDelegate OnAddToInvDelegate;

private:
	virtual void BeginPlay() override;

	void DestroyMagazine();

	void AddMagazine();

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

};
