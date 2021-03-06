// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ReplicableObject.generated.h"

/**
 * Object with support of replication and RPC's.
 */
UCLASS(BlueprintType, Blueprintable)
class ADVANCEDOBJECTPLUGIN_API UReplicableObject : public UObject
{
	GENERATED_BODY()

public:

	UReplicableObject();

	// Override for the initial setup of this object.
	virtual void PostInitProperties() override;

	// Overrides for the support of replicable propeerties.
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Overrides for the support of RPC's
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, struct FOutParmRec* OutParameters, FFrame* Stack) override;
 	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;

	/**
	 * Allows to replicate sub-ReplicableObjects that have been marked for replication.
	 */
	virtual void ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags, bool& OutWroteSomething);

	/**
	 * Whether this object is currently being replicated.
	 * @return Is this object replicating now.
	 */
	bool GetIsReplicatingNow() const { return bIsReplicatingNow; }

protected:

	/**
	 * The first outer actor that was found by following the chain of outers for this object.
	 * @return The first outer actor.
	 */
	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "ReplicableObject")
	class AActor* GetFirstOuterActor() const { return FirstOuterActor; }

	/**
	 * Returns whether the owner actor of this object has network authority.
	 * @return Has this object authority.
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintProtected), Category = "ReplicableObject")
	bool HasAuthority() const;

	/**
	 * Returns the NetRole of the owner actor of this object.
	 * @return The net role of the owner.
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintProtected), Category = "ReplicableObject")
	ENetRole GetNetRole() const;

	/**
	 * Set is this object replicating now.
	 * @param bNewIsReplicatingNow - The new value.
	 */
	void SetIsReplicatingNow(bool bNewIsReplicatingNow) { bIsReplicatingNow = bNewIsReplicatingNow; }

private:

	UPROPERTY()
	class UReplicator* Replicator = nullptr;

	UPROPERTY(BlueprintGetter = GetFirstOuterActor, Category = "ReplicableObject")
	class AActor* FirstOuterActor = nullptr;

	class AActor* FindFirstOuterActor();

	bool bIsReplicatingNow = false;

};
