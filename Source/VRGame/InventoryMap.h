
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.h"
#include "InventoryMap.generated.h"

UCLASS()
class VRGAME_API AInventoryMap : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventoryMap();

	void AddObject(EInventoryObjectTypes Type);

	void RemoveObject(EInventoryObjectTypes Type);

	bool IsInInventory(EInventoryObjectTypes Type);

private:
	virtual void BeginPlay() override;

	TMap<EInventoryObjectTypes, int> inventoryMap;
};
