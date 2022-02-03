Scriptname ESIF_ReplaceItemScript extends ObjectReference
{Script for replacing an item unaggressively.}

Form Property OriginalItem Auto
{The item to be replaced.}

Form Property ReplacementItem Auto
{The item that should replace the original.}

Event OnContainerChanged(ObjectReference akNewContainer, ObjectReference akOldContainer)
	; Don't try replacing if persistent, or if there is already more than 1
	if Self.GetBaseObject() == None && akNewContainer.GetItemCount(OriginalItem) == 1
		; Don't remove equipped items
		Actor kActor = akNewContainer as Actor
		if !kActor || !kActor.IsEquipped(OriginalItem)
			akNewContainer.RemoveItem(OriginalItem, 1, abSilent = true)
			akNewContainer.AddItem(ReplacementItem, 1, abSilent = true)
		endif
	endif
EndEvent
