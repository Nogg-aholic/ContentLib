

#pragma once

#include "CoreMinimal.h"

#include "FGSchematic.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CLRecipeBPFLib.generated.h"

class UFGRecipe;
class UContentLibSubsystem;


USTRUCT(BlueprintType)
struct CONTENTLIB_API FJsonRecipe
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UFGRecipe> Class;
	UPROPERTY(BlueprintReadWrite)
	FString Json;
};

USTRUCT(BlueprintType)
struct CONTENTLIB_API FJsonSchematic
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UFGSchematic> Class;
	UPROPERTY(BlueprintReadWrite)
	FString Json;
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_Recipe
{
	GENERATED_BODY()
	FContentLib_Recipe(): OverrideName(-1),
	                      ManufacturingDuration(-1),
	                      ManualManufacturingMultiplier(-1),
	                      VariablePowerConsumptionFactor(-1),
	                      VariablePowerConsumptionConstant(-1),
	                      ManufacturingMenuPriority(-1),
	                      ClearIngredients(true),
	                      ClearProducts(true),
	                      ClearBuilders(true)
	{
	} ;

	UPROPERTY(BlueprintReadWrite)
		int32 OverrideName;

	UPROPERTY(BlueprintReadWrite)
		FString Name;

	UPROPERTY(BlueprintReadWrite)
		FString Category;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, int32> Ingredients;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, int32> Products;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> BuildIn;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> UnlockedBy;
	UPROPERTY(BlueprintReadWrite)
		float ManufacturingDuration;
	UPROPERTY(BlueprintReadWrite)
		float ManualManufacturingMultiplier;
	UPROPERTY(BlueprintReadWrite)
		float VariablePowerConsumptionFactor;
	UPROPERTY(BlueprintReadWrite)
		float VariablePowerConsumptionConstant;
	UPROPERTY(BlueprintReadWrite)
		float ManufacturingMenuPriority;
	UPROPERTY(BlueprintReadWrite)
		bool ClearIngredients;
	UPROPERTY(BlueprintReadWrite)
		bool ClearProducts;
	UPROPERTY(BlueprintReadWrite)
		bool ClearBuilders;
};



/**
 * 
 */
UCLASS()
class CONTENTLIB_API UCLRecipeBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void InitRecipeFromStruct(UContentLibSubsystem* Subsystem ,FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe, bool ClearIngredients = true, bool ClearProducts = true, bool ClearBuilders = true);
	UFUNCTION(BlueprintCallable)
	static void AddToSchematicUnlock(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct, UContentLibSubsystem* Subsystem);
	UFUNCTION(BlueprintCallable)
	static void AddBuilders(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, bool ClearFirst = false);


	UFUNCTION(BlueprintCallable)
	static FContentLib_Recipe GenerateCLRecipeFromString(FString String);
	UFUNCTION(BlueprintCallable)
	static FString SerializeRecipe(TSubclassOf<UFGRecipe> Recipe);
	UFUNCTION(BlueprintCallable)
    static FString SerializeCLRecipe(FContentLib_Recipe Recipe);
};
