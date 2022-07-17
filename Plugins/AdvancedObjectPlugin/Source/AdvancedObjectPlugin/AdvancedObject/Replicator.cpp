// Fill out your copyright notice in the Description page of Project Settings.


#include "Replicator.h"

#include "Engine/ActorChannel.h"
#include "ReplicableObject.h"
#include "Kismet/KismetSystemLibrary.h"

void UReplicator::PostInitProperties()
{
	Super::PostInitProperties();

	UClass* Class = GetOuter()->GetClass();
	FindPropertiesForReplication(Class);
}

void UReplicator::ReplicateSubobjectsOfOwner(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething)
{
	ReplicateObjects(Channel, Bunch, RepFlags, OutWroteSomething);
	ReplicateArrays(Channel, Bunch, RepFlags, OutWroteSomething);
}

void UReplicator::FindPropertiesForReplication(UClass* Class)
{
	// Create an iterator for object properties of the class.
	TFieldIterator<FObjectProperty> ObjectsIterator(Class);
	// Find replicable properties.
	ObjectProperties = FindObjectPropertiesForReplication(ObjectsIterator);

	// Create an iterator for array properties of the class.
	TFieldIterator<FArrayProperty> ArraysIterator(Class);
	// Find replicable properties.
	ArrayProperties = FindArrayPropertiesForReplication(ArraysIterator);

	// Create an iterator for struct properties of the class.
	TFieldIterator<FStructProperty> StructsIterator(Class);
	// Find replicable properties.
	ObjectProperties.Append(FindObjectPropertiesInStructsForReplication(StructsIterator));
}

TArray<FObjectProperty*> UReplicator::FindObjectPropertiesForReplication(TFieldIterator<FObjectProperty> Iterator)
{
	TArray<FObjectProperty*> OutProperties;
	for(; Iterator; ++Iterator)
	{
		// Get the current object property.
		FObjectProperty* ObjectProperty = *Iterator;

		// If the property has the Replicated or RepNotify flags...
		if((ObjectProperty->GetPropertyFlags() & EPropertyFlags::CPF_Net) || (ObjectProperty->GetPropertyFlags() & EPropertyFlags::CPF_RepNotify))
		{
			// then add it to the array.
			OutProperties.Add(ObjectProperty);
		}
	}

	return OutProperties;
}
	
TArray<FArrayProperty*> UReplicator::FindArrayPropertiesForReplication(TFieldIterator<FArrayProperty> Iterator)
{
	TArray<FArrayProperty*> OutProperties;
	for(; Iterator; ++Iterator)
	{
		// Get the current array property.
		FArrayProperty* ArrayProperty = *Iterator;

		// If the property has the Replicated or RepNotify flags...
		if((ArrayProperty->GetPropertyFlags() & EPropertyFlags::CPF_Net) || (ArrayProperty->GetPropertyFlags() & EPropertyFlags::CPF_RepNotify))
		{
			// then add it to the array.
			OutProperties.Add(ArrayProperty);
		}
	}

	return OutProperties;
}

TArray<FObjectProperty*> UReplicator::FindObjectPropertiesInStructsForReplication(TFieldIterator<FStructProperty> Iterator)
{
	TArray<FObjectProperty*> OutProperties;
	for(; Iterator; ++Iterator)
	{
		// Get the current struct property.
		FStructProperty* StructProperty = *Iterator;

		// If the property has the Replicated or RepNotify flags...
		if ((StructProperty->GetPropertyFlags() & EPropertyFlags::CPF_Net) || (StructProperty->GetPropertyFlags() & EPropertyFlags::CPF_RepNotify))
		{
			// then parse it and add the object properties to the array.
			TFieldIterator<FObjectProperty> ObjectsIterator(StructProperty->Struct);
			for (; ObjectsIterator; ++ObjectsIterator)
			{
				// Get and add the current object property.
				FObjectProperty* ObjectProperty = *ObjectsIterator;
				OutProperties.Add(ObjectProperty);
			}
		}
	}
	
	return OutProperties;
}

void UReplicator::ReplicateObjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething)
{
	for (FObjectProperty* ObjectProperty : ObjectProperties)
	{
		// Get the property value.
		UObject* Object = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<UObject>(GetOuter());
		
		ReplicateObject(Object, Channel, Bunch, RepFlags, OutWroteSomething);
	}
}

void UReplicator::ReplicateArrays(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething)
{
	for(FArrayProperty* ArrayProperty : ArrayProperties)
	{
		// Get the property value.
		TArray<UObject*> Array = *ArrayProperty->ContainerPtrToValuePtr<TArray<UObject*>>(GetOuter());
		
		for(UObject* Object : Array)
		{
			ReplicateObject(Object, Channel, Bunch, RepFlags, OutWroteSomething);
		}
	}
}

void UReplicator::ReplicateObject(UObject* Object, UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething)
{
	// If the object is replicable and is not being replicated already...
	UReplicableObject* ReplicableObject = Cast<UReplicableObject>(Object);

	if(IsValid(ReplicableObject))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), Object->GetName());
	}

	if(IsValid(ReplicableObject) && !ReplicableObject->GetIsReplicatingNow())
	{
		// then replicate it...
		OutWroteSomething |= Channel->ReplicateSubobject(ReplicableObject, *Bunch, *RepFlags);
		// and its subobjects.
		ReplicableObject->ReplicateSubobjects(Channel, Bunch, RepFlags, OutWroteSomething);

		UKismetSystemLibrary::PrintString(GetWorld(), Object->GetName());
	}
}
