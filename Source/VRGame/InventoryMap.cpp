
#include "InventoryMap.h"


AInventoryMap::AInventoryMap()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AInventoryMap::AddObject(EInventoryObjectTypes Type)
{
	if (IsInInventory(Type))
	{
		int val = inventoryMap[Type] + 1;
		inventoryMap.Emplace(Type, val);
	}
	else
	{
		inventoryMap.Add(Type, 1);
	}
}

void AInventoryMap::RemoveObject(EInventoryObjectTypes Type)
{
	int val = inventoryMap[Type] - 1;

	if (val >= 0)
	{
		inventoryMap.Emplace(Type, val);
	}
}

bool AInventoryMap::IsInInventory(EInventoryObjectTypes Type)
{
	if (inventoryMap.Contains(Type))
	{
		int count = inventoryMap[Type];
		if (count > 0)
		{
			return true;
		}
	}
}

