
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

private:
	virtual void BeginPlay() override;

};
