
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryObjectTypes.h"
#include "Inventory.generated.h"

UCLASS()
class VRGAME_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventory();

	void AddObject(EInventoryObjectTypes Type);

	void RemoveObject(EInventoryObjectTypes Type);

	bool IsInInventory(EInventoryObjectTypes Type);

private:
	TMap<EInventoryObjectTypes, int> inventoryMap;
};
