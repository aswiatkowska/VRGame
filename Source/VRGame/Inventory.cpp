
#include "Inventory.h"


AInventory::AInventory()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AInventory::AddObject(EInventoryObjectTypes Type)
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

void AInventory::RemoveObject(EInventoryObjectTypes Type)
{
	int val = inventoryMap[Type] - 1;

	if (val >= 0)
	{
		inventoryMap.Emplace(Type, val);
	}
}

bool AInventory::IsInInventory(EInventoryObjectTypes Type)
{
	if (inventoryMap.Contains(Type))
	{
		int count = inventoryMap[Type];
		if (count > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
	return false;
	}
}

