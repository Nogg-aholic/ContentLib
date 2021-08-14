


#include "CLUtilBPFLib.h"


#include "BPFContentLib.h"
#include "ContentLibSubsystem.h"


void UCLUtilBPFLib::SortPairs(TArray<TSubclassOf<UObject>>& Array_To_Sort_Keys, TArray<float>& Array_To_Sort_Values, const bool Descending)
{
	const int32 m = Array_To_Sort_Keys.Num();      // Return the array size

	for (int32 a = 0; a < (m - 1); a++)
	{
		bool bDidSwap = false;

		if (Descending == true)         // If element 1 is less than element 2 move it back in the array (sorts high to low)
		{
			for (int32 k = 0; k < m - a - 1; k++)
			{
				if (Array_To_Sort_Values[k] < Array_To_Sort_Values[k + 1])
				{
					const float z = Array_To_Sort_Values[k];
					const auto Ob = Array_To_Sort_Keys[k];
					Array_To_Sort_Values[k] = Array_To_Sort_Values[k + 1];
					Array_To_Sort_Keys[k] = Array_To_Sort_Keys[k + 1];
					Array_To_Sort_Values[k + 1] = z;
					Array_To_Sort_Keys[k + 1] = Ob;
					bDidSwap = true;
				}
			}

			if (bDidSwap == false)      // If no swaps occured array is sorted do not go through last loop
			{
				break;
			}
		}
		else
		{
			for (int32 k = 0; k < m - a - 1; k++)
			{
				if (Array_To_Sort_Values[k] > Array_To_Sort_Values[k + 1])
				{
					const float z = Array_To_Sort_Values[k];
					const auto Ob = Array_To_Sort_Keys[k];
					Array_To_Sort_Values[k] = Array_To_Sort_Values[k + 1];
					Array_To_Sort_Keys[k] = Array_To_Sort_Keys[k + 1];
					Array_To_Sort_Values[k + 1] = z;
					Array_To_Sort_Keys[k + 1] = Ob;
					bDidSwap = true;
				}
			}

			if (bDidSwap == false)      // If no swaps occured array is sorted do not go through last loop
			{
				break;
			}
		}
	}
}

TMap<TSubclassOf<UFGItemDescriptor>, FFactoryGame_Descriptor>  UCLUtilBPFLib::CalculateRecipesRecursively(const TSubclassOf<UFGItemDescriptor> Item, const TArray<TSubclassOf<UFGRecipe>> Exclude, const bool UseAlternates, UContentLibSubsystem* System)
{
	TMap<TSubclassOf<UFGItemDescriptor>, FFactoryGame_Descriptor>  Map;
	TArray<TSubclassOf<UFGRecipe>> nRecipes;
	TArray<TSubclassOf<UFGItemDescriptor>> RecipesItem;
	RecurseIngredients(Item, RecipesItem, nRecipes, System, UseAlternates, Exclude);

	for (auto i : RecipesItem)
	{
		System->Items.Find(i)->MJValue = -1;
	}
	for (auto i : nRecipes)
	{
		System->Recipes.Find(i)->MJ.MJ_Average = 0;
	}
	CalculateCost(nRecipes,System);
	for (auto i : RecipesItem)
	{
		FFactoryGame_Descriptor& InValue = *System->Items.Find(i);
		Map.Add(i, InValue);
	}
	return Map;
};

void UCLUtilBPFLib::CalculateCost(TArray<TSubclassOf<UFGRecipe>> RecipesToCalc,UContentLibSubsystem* System)
{
	UE_LOG(LogTemp, Display, TEXT("******************** Content Info MJ Calculation %i Recipes to Calculate ********************"), RecipesToCalc.Num());
	int32 CounterInside = 0;
	int8  Loops = 0;
	while (CounterInside < RecipesToCalc.Num())
	{
		for (auto Recipe : RecipesToCalc)
		{
			FFactoryGame_Recipe& ItemItr = *System->Recipes.Find(Recipe);
			if (ItemItr.MJ.TryAssignMJ(System))
				CounterInside++;
		}
		if (Loops < 20)
		{
			Loops++;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("******************** Content Info MJ Calculation failed, Had to stop after 20 LoopCyles, Result : %i / %i ********************"), CounterInside, RecipesToCalc.Num());
			return;
		}
	}
}

bool UCLUtilBPFLib::FindVisualKit(const FString Name, FContentLib_VisualKit& Kit,UContentLibSubsystem* System)
{
	for (const auto i : System->ImportedVisualKits)
	{
		if (UBPFContentLib::StringCompareItem(Name,i.Key,"Desc","_C"))
		{
			Kit = i.Value;
			return true;
		}
	}
	for (const auto i : System->VisualKits)
	{
		if (UBPFContentLib::StringCompareItem(Name,i.Key,"Desc","_C"))
		{
			Kit = i.Value;
			return true;
		}
	}
	Kit = FContentLib_VisualKit();
	return false;
}


void UCLUtilBPFLib::PrintSortedRecipes(UContentLibSubsystem* System)
{
	TArray<TSubclassOf<UObject>> Array_To_Sort_Keys;
	TArray<float> Array_To_Sort_Values;
	for(auto i: System->Recipes)
	{
		if(i.Value.MJ.HasAssignedMJ())
		{
			Array_To_Sort_Keys.Add(i.Key);
			Array_To_Sort_Values.Add(i.Value.MJ.MJ_Average);
		}
	}
	SortPairs(Array_To_Sort_Keys,Array_To_Sort_Values);
	for(int32 i = 0; i< Array_To_Sort_Keys.Num(); i++)
	{
		UE_LOG(LogTemp, Display,TEXT("Recipe: %s MJ: %f"), *Array_To_Sort_Keys[i]->GetName(),Array_To_Sort_Values[i]);
	}
}

void UCLUtilBPFLib::PrintSortedItems(UContentLibSubsystem* System)
{
	TArray<TSubclassOf<UObject>> Array_To_Sort_Keys;
	TArray<float> Array_To_Sort_Values;
	for(auto i: System->Items)
	{
		if(i.Value.HasMj())
		{
			Array_To_Sort_Keys.Add(i.Key);
			Array_To_Sort_Values.Add(i.Value.MJValue);
		}
	}
	SortPairs(Array_To_Sort_Keys,Array_To_Sort_Values);
	for(int32 i = 0; i< Array_To_Sort_Keys.Num(); i++)
	{
		UE_LOG(LogTemp,Display,TEXT("Item: %s MJ: %f"), *Array_To_Sort_Keys[i]->GetName(),Array_To_Sort_Values[i]);
	}
};


int32 UCLUtilBPFLib::CalculateDepth(UContentLibSubsystem * System, const TSubclassOf<UFGItemDescriptor> Item)
{
	if (Item)
	{
		const TArray<TSubclassOf<UFGRecipe>> Exc;
		TArray<TSubclassOf<UFGRecipe>> Recipes;
		TArray<TSubclassOf<UFGItemDescriptor>> RecipesItem;
		RecurseIngredients(Item,RecipesItem,Recipes,System,false,Exc,true);
		return Recipes.Num();
	}
	return 0 ;
}

void UCLUtilBPFLib::RecurseIngredients(const TSubclassOf<class UFGItemDescriptor> Item, TArray<TSubclassOf<class UFGItemDescriptor>> & AllItems , TArray<TSubclassOf<class UFGRecipe>> & AllRecipes ,UContentLibSubsystem * System, bool SkipAlternate, TArray<TSubclassOf<class UFGRecipe>> Excluded, bool UseFirst)
{
	if(!System)
		return;
	
	const int32 Len = AllRecipes.Num(); 
	for(auto i : System->Items.Find(Item)->ProductInRecipe)
	{
		if(!Excluded.Contains(i))
			continue;
		if(!AllRecipes.Contains(i))
			AllRecipes.Add(i);
		FFactoryGame_Recipe Recipe = *System->Recipes.Find(i); 
		if(Recipe.UnlockedFromAlternate() && SkipAlternate)
			continue;
		
		for(auto e: Recipe.Ingredients())
		{
			if(!AllRecipes.Contains(i))
				RecurseIngredients(e, AllItems,AllRecipes,System, SkipAlternate,Excluded);
			if(!AllItems.Contains(e))
				AllItems.Add(e);
		}
		if(UseFirst && Len != AllRecipes.Num())
			return;
	}
};

void UCLUtilBPFLib::AddToSchematicArrayProp(UPARAM(ref)FFactoryGame_Schematic& Obj,
                                                    const TSubclassOf<UFGSchematic> Schematic, const int32 Index)
{
	if (Index == 0)
	{
		Obj.nUnlockedBy.Add(Schematic);
	}
	else if (Index == 1)
	{
		Obj.nDependsOn.Add(Schematic);
	}
	else if (Index == 2)
	{
		Obj.nDependingOnThis.Add(Schematic);
	}
	else if (Index == 3)
	{
		Obj.nVisibilityDepOn.Add(Schematic);
	}
	else if (Index == 4)
	{
		Obj.nVisibilityDepOnThis.Add(Schematic);
	}
}