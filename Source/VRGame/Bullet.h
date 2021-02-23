
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Bullet.generated.h"

UCLASS()
class VRGAME_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* BulletArrowComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletForce = 30.0f;

private:
	virtual void BeginPlay() override;

};
