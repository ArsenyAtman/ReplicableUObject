// Fill out your copyright notice in the Description page of Project Settings.


#include "Replicator.h"

#include "Engine/ActorChannel.h"
#include "ReplicableObject.h"
#include "Kismet/KismetSystemLibrary.h"

void UReplicator::Initialize(class UObject* ReplicatorOwner)
{
	Owner = ReplicatorOwner;

	UClass* Class = Owner->GetClass();
	FindPropertiesForReplication(Class);

}

void UReplicator::ReplicateSubobjectsOfOwner(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething)
{
	for (FObjectProperty* ObjectProperty : ObjectProperties)
	{
		UObject* Object = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<UObject>(Owner));
		OutWroteSomething |= Channel->ReplicateSubobject(Object, *Bunch, *RepFlags);

		UReplicableObject* ReplicableObject = Cast<UReplicableObject>(Object);
		if(IsValid(ReplicableObject))
		{
			ReplicableObject->ReplicateSubobjects(Channel, Bunch, RepFlags, OutWroteSomething);
			//UKismetSystemLibrary::PrintString(GetWorld(), Owner->GetName() + " ReplSubobjects");
		}

		//UKismetSystemLibrary::PrintString(GetWorld(), Owner->GetName() + " Repl");
	}

	for(FArrayProperty* ArrayProperty : ArrayProperties)
	{
		TArray<UObject*> Array = *ArrayProperty->ContainerPtrToValuePtr<TArray<UObject*>>(Owner);
		OutWroteSomething |= Channel->ReplicateSubobjectList(Array, *Bunch, *RepFlags);

		for(UObject* Object : Array)
		{
			UReplicableObject* ReplicableObject = Cast<UReplicableObject>(Object);
			if(IsValid(ReplicableObject))
			{
				ReplicableObject->ReplicateSubobjects(Channel, Bunch, RepFlags, OutWroteSomething);
			}
		}
	}
}

void UReplicator::FindPropertiesForReplication(UClass* Class)
{
	TFieldIterator<FObjectProperty> ObjectsIterator(Class);
	ObjectProperties = FindObjectPropertiesForReplication(ObjectsIterator);

	TFieldIterator<FArrayProperty> ArraysIterator(Class);
	ArrayProperties = FindArrayPropertiesForReplication(ArraysIterator);
}

TArray<FObjectProperty*> UReplicator::FindObjectPropertiesForReplication(TFieldIterator<FObjectProperty> Iterator)
{
	TArray<FObjectProperty*> Properties;
	for(; Iterator; ++Iterator)
	{
		FObjectProperty* ObjectProperty = *Iterator;
		if((ObjectProperty->GetPropertyFlags() & EPropertyFlags::CPF_Net) || (ObjectProperty->GetPropertyFlags() & EPropertyFlags::CPF_RepNotify))
		{
			Properties.Add(ObjectProperty);
		}
	}

	return Properties;
}
	
TArray<FArrayProperty*> UReplicator::FindArrayPropertiesForReplication(TFieldIterator<FArrayProperty> Iterator)
{
	TArray<FArrayProperty*> Properties;
	for(; Iterator; ++Iterator)
	{
		FArrayProperty* ArrayProperty = *Iterator;
		if((ArrayProperty->GetPropertyFlags() & EPropertyFlags::CPF_Net) || (ArrayProperty->GetPropertyFlags() & EPropertyFlags::CPF_RepNotify))
		{
			Properties.Add(ArrayProperty);
		}
	}

	return Properties;
}