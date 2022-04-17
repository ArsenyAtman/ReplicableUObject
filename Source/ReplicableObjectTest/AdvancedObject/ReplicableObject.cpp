// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicableObject.h"

#include "ReplicatingActorComponent.h"
#include "Replicator.h"
#include "Kismet/KismetSystemLibrary.h"

void UReplicableObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
    if (BPClass)
    {
        BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
    }
}

void UReplicableObject::PostInitProperties()
{
    Super::PostInitProperties();

    FirstOuterActor = FindFirstOuterActor();

    if(GetFirstOuterActor() && GetFirstOuterActor()->GetWorld())
    {
        UKismetSystemLibrary::PrintString(GetWorld(), GetName() + " CreateRepl");
        Replicator = NewObject<UReplicator>(this);
        Replicator->Initialize(this);
    }
    
}

void UReplicableObject::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething)
{
    Replicator->ReplicateSubobjectsOfOwner(Channel, Bunch, RepFlags, OutWroteSomething);
}

bool UReplicableObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParameters, FFrame* Stack)
{
    AActor* OuterActor = GetFirstOuterActor();
    if (IsValid(OuterActor))
    {
        UNetDriver* NetDriver = OuterActor->GetNetDriver();
        if (IsValid(NetDriver))
        {
            NetDriver->ProcessRemoteFunction(OuterActor, Function, Parameters, OutParameters, Stack, this);
            return true;
        }
    }
    return false;
}

int32 UReplicableObject::GetFunctionCallspace(UFunction * Function, FFrame * Stack)
{
    AActor* OuterActor = GetFirstOuterActor();
    if (IsValid(OuterActor))
    {
        return OuterActor->GetFunctionCallspace(Function, Stack);
    }
 
    return FunctionCallspace::Local;
}

AActor* UReplicableObject::FindFirstOuterActor()
{
    UObject* Outer = GetOuter();
    while(IsValid(Outer))
    {
        UActorComponent* ActorComponent = Cast<UActorComponent>(Outer);
        if(IsValid(ActorComponent))
        {
            return ActorComponent->GetOwner();
        }
        
        AActor* Actor = Cast<AActor>(Outer);
        if (IsValid(Actor))
        {
            return Actor;
        }

        Outer = Outer->GetOuter();
    }

    return nullptr;
}