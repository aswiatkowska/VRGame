
#include "InventoryMap.h"


AInventoryMap::AInventoryMap()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AInventoryMap::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInventoryMap::AddObject(EInventoryObjectTypes Type)
{
	if (IsInInventory(Type))
	{
		int val = inventoryMap[Type];
		inventoryMap.Emplace(Type, val++);
	}
	else
	{
		inventoryMap.Add(Type, 1);
	}
}

void AInventoryMap::RemoveObject(EInventoryObjectTypes Type)
{
	int val = inventoryMap[Type];

	if ( val > 1)
	{
		inventoryMap.Emplace(Type, (val - 1));
	}
	else
	{
		inventoryMap.Remove(Type);
	}
}

bool AInventoryMap::IsInInventory(EInventoryObjectTypes Type)
{
	return inventoryMap.Contains(Type);
}

