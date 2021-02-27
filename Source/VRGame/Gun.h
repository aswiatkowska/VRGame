
#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"


UCLASS()
class VRGAME_API AGun : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();

	virtual bool ShootingSpree() override;
};
