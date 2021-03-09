
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableObjectComponent.h"
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
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, Category = GrabbableObject)
	TEnumAsByte<EObjectToGrab::Type> GrabbableObjType;

	void DestroyMagazine();

};
