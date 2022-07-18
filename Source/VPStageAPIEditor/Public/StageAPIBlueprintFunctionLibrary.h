#pragma once

#include "CoreMinimal.h"
#include "API/IStageAPIEditor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StageAPIBlueprintFunctionLibrary.generated.h"

UCLASS()
class VPSTAGEAPIEDITOR_API UStageAPIBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get VP Stage API"), Category = "VP Stage API")
	static void GetAPI(TScriptInterface<IStageAPIEditor>& OutAPI);
	
};
