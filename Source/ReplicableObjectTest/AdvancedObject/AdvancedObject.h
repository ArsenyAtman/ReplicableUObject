// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicableObject.h"
#include "Tickable.h"
#include "AdvancedObject.generated.h"

/**
 * 
 */
UCLASS()
class REPLICABLEOBJECTTEST_API UAdvancedObject : public UReplicableObject, public FTickableGameObject
{
	GENERATED_BODY()
	
public:

	// overrides for EventTick
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	// override for BeginPlay and EventConstruct
	virtual void PostInitProperties() override;

	// override for EndPlay
	virtual void BeginDestroy() override;

	virtual class UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable)
	void Destroy();

protected:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginPlay();
	virtual void BeginPlay_Implementation() { return; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndPlay();
	virtual void EndPlay_Implementation() { return; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ObjectTick(float DeltaTime);
	virtual void ObjectTick_Implementation(float DeltaTime) { return; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanEverTick = true;

};
