
#include "GrabbableObjectComponent.h"

UGrabbableObjectComponent::UGrabbableObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGrabbableObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UGrabbableObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

