#include "StageAPIBlueprintFunctionLibrary.h"
#include "../Private/API/StageAPIEditorImpl.h"

void UStageAPIBlueprintFunctionLibrary::GetAPI(TScriptInterface<IStageAPIEditor>& OutAPI)
{
	static UStageAPIImpl* Obj = NewObject<UStageAPIImpl>(GetTransientPackage(),NAME_None,RF_MarkAsRootSet);
	OutAPI = Obj;
}
