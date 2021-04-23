
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObjectComponent.h"
#include "InventoryObjectTypes.h"
#include "MyCharacter.h"
#include "Magazine.generated.h"

UCLASS()
class VRGAME_API AMagazine : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagazine();

	void DestroyMagazine();

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

	UFUNCTION()
	void OnGrab();

	UFUNCTION()
	void OnRelease();

	AMyCharacter* MyCharacter;

};
