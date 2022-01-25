

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CLCDOBPFLib.generated.h"


/**
 * 
 */
UCLASS()
class CONTENTLIB_API UCLCDOBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static void Log(FString LogString, int32 Level = 0);
	UFUNCTION(BlueprintCallable)
    static bool GenerateCLCDOFromString(FString String, bool DoLog);
	static void EditCDO(FProperty * Prop, TSharedPtr<FJsonValue> json,bool DoLog,void * Ptr);

	static UClass* FindClassByName(FString ClassNameInput);
};
