

#pragma once

#include "CoreMinimal.h"


#include "ContentLibSubsystem.h"
#include "FGBuildCategory.h"
#include "FGItemCategory.h"
#include "FGSchematicCategory.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CLCategoryBPFLib.generated.h"


USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_ItemCategory
{
	GENERATED_BODY()
	FContentLib_ItemCategory(): MenuPriority(0)
	{
	};
	
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	
	UPROPERTY(BlueprintReadWrite)
	float MenuPriority;

};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_SchematicCategory
{
	GENERATED_BODY()
	FContentLib_SchematicCategory()
	{
	};
	
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	
	UPROPERTY(BlueprintReadWrite)
	FString Icon;

};

/**
 * 
 */
UCLASS()
class CONTENTLIB_API UCLCategoryBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	UFUNCTION(BlueprintCallable)
    static FString GenerateFromItemCategory(TSubclassOf<UFGItemCategory> Item);
	UFUNCTION(BlueprintCallable)
    static FContentLib_ItemCategory CLItemCategoryFromString(FString String);

	UFUNCTION(BlueprintCallable)
    static FString GenerateFromSchematicCategory(TSubclassOf<UFGSchematicCategory> Item);
	UFUNCTION(BlueprintCallable)
    static FContentLib_SchematicCategory CLSchematicCategoryFromString(FString String, UContentLibSubsystem* Subsystem);
	UFUNCTION(BlueprintCallable)
	static void InitItemCategoryFromStruct(FContentLib_ItemCategory SchematicCategory, TSubclassOf<UFGItemCategory> SchematicCategoryClass,  UContentLibSubsystem* Subsystem);
	UFUNCTION(BlueprintCallable)
	static void InitSchematicCategoryFromStruct(FContentLib_SchematicCategory Schematic, TSubclassOf<UFGSchematicCategory> SchematicClass,   UContentLibSubsystem* SubSystem);
	UFUNCTION(BlueprintCallable)
	static void InitBuildCategoryFromStruct(FContentLib_SchematicCategory SchematicCategory, TSubclassOf<UFGBuildCategory> BuildCategoryClass, UContentLibSubsystem* Subsystem);
	UFUNCTION(BlueprintCallable)
	static void ApplyVisualKitToPurchaseCategory(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit, TSubclassOf<UFGBuildCategory> Item);
	UFUNCTION(BlueprintCallable)
	static void ApplyVisualKitToSchematicCategory(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit,  TSubclassOf<UFGSchematicCategory> Item);
};
