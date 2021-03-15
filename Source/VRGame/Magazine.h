
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObjectComponent.h"
#include "Inventory.h"
#include "MyCharacter.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EInventoryObjectTypes> InvObjectType;

private:
	virtual void BeginPlay() override;

	void DestroyMagazine();

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

	AMyCharacter* MyCharacter;

};
