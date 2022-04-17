# ReplicableUObject

Developed with UnrealEngine 5.

My own implementation of a replicable object.

There are two main features of the ReplicableObject class:
1) The system automatically collects all the properties that need to be replicated and passes them to the actor channel of the first outer actor. For the end-user this means that the replication setup process is the same as for any actor or actor component.
2) ReplicableObjects in this system can be any level of nesting. I.e. all replicable objects will be replicated with their own replicable subobjects, and so on.

The only one restriction is that object replication is possible only for UPROPERTIES of the ReplicatingActorComponent class.

Short description of each class:
- ReplicatingActorComponent - an actor component that provides replication for its subobjects.
- ReplicableObject - an object that replicates with its subobjects.
- Replicator - an object that parses properties of its owner and passes them to an ActorChannel of the first outer actor during the repllication process.
- AdvancedObjet - a child class of the ReplicableObject, that implements BeginPlay, EndPlay, Tick, Destroy and GetWorld methods.
