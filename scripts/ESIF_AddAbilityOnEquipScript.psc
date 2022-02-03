Scriptname ESIF_AddAbilityOnEquipScript extends ObjectReference
{Script for adding an ability to an actor when an item is equipped.}

Spell Property AbilityToAdd Auto
{The ability that should be added when the item is equipped.}

Form Property BaseObject Auto
{The base item (must be supplied to handle non-persistent items).}

Event OnInit()
	if !BaseObject
		BaseObject = Self.GetBaseObject()
	endif
EndEvent

Event OnContainerChanged(ObjectReference akNewContainer, ObjectReference akOldContainer)
	; Wearing an item from a container doesn't produce an event, so handle it separately
	Actor kActor = akNewContainer as Actor
	if kActor
		TryAdd(kActor, false)
	endif
EndEvent

Event OnEquipped(Actor akActor)
	TryAdd(akActor, true)
EndEvent

Event OnUnequipped(Actor akActor)
	akActor.RemoveSpell(AbilityToAdd)
EndEvent

Function TryAdd(Actor akActor, bool abEquipEvent)
{Add the ability if the item is equipped.}
	bool bIsEquipped = abEquipEvent

	if BaseObject
		; Delay to make sure the object stays equipped and allow other instances to clean up
		Utility.Wait(0.2)
		bIsEquipped = akActor.IsEquipped(BaseObject)
	endif

	if bIsEquipped
		akActor.AddSpell(AbilityToAdd, abVerbose = false)
	endif
EndFunction
