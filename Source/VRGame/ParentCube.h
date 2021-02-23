
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParentCube.generated.h"

UCLASS()
class VRGAME_API AParentCube : public AActor
{
	GENERATED_BODY()
	
public:	
	AParentCube();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CubeMesh;

protected:
	virtual void BeginPlay() override;

	virtual void OnDestroy();

	UFUNCTION()
	virtual void OnOverlap(AActor* OverlappedActor, AActor* OtherActor);

};
