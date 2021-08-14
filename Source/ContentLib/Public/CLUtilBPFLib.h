

#pragma once

#include "CoreMinimal.h"

#include "FGRecipe.h"
#include "FGSchematic.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Resources/FGItemDescriptor.h"

#include "CLUtilBPFLib.generated.h"

struct FFactoryGame_Schematic;
struct FContentLib_VisualKit;
struct FFactoryGame_Descriptor;
class UContentLibSubsystem;
/**
 * 
 */
UCLASS()
class CONTENTLIB_API UCLUtilBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
		static void SortPairs(UPARAM(ref)TArray<TSubclassOf<UObject>>& Array_To_Sort_Keys, UPARAM(ref)TArray<float>& Array_To_Sort_Values, bool Descending = true);

	UFUNCTION(BlueprintCallable)
		static TMap<TSubclassOf<UFGItemDescriptor>, FFactoryGame_Descriptor> CalculateRecipesRecursively(TSubclassOf<UFGItemDescriptor> Item, TArray<TSubclassOf<UFGRecipe>> Exclude, bool UseAlternates, UContentLibSubsystem* System);


	UFUNCTION(BlueprintCallable)
		static void CalculateCost(TArray<TSubclassOf<UFGRecipe>> RecipesToCalc, UContentLibSubsystem* System);


	UFUNCTION(BlueprintCallable)
	static bool FindVisualKit(FString Name, UPARAM(ref)FContentLib_VisualKit & Kit, UContentLibSubsystem* System);

	UFUNCTION(BlueprintCallable)
	static void PrintSortedRecipes( UContentLibSubsystem* System);
	UFUNCTION(BlueprintCallable)
	static void PrintSortedItems( UContentLibSubsystem* System);

	UFUNCTION(BlueprintCallable)
	static void RecurseIngredients(TSubclassOf<class UFGItemDescriptor> Item , UPARAM(ref)TArray<TSubclassOf<class UFGItemDescriptor>> & AllItems , UPARAM(ref)TArray<TSubclassOf<class UFGRecipe>> & AllRecipes ,UContentLibSubsystem * System, bool SkipAlternate, TArray<TSubclassOf<class UFGRecipe>> Excluded, bool UseFirst = false);

	UFUNCTION(BlueprintCallable)
	static int32 CalculateDepth(UContentLibSubsystem* System, TSubclassOf<UFGItemDescriptor> Item);

	UFUNCTION(BlueprintCallable)
    static void AddToSchematicArrayProp(UPARAM(ref)FFactoryGame_Schematic& Obj, TSubclassOf<UFGSchematic> Schematic, int32 Index);

};
