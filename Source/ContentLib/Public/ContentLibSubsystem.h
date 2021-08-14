

#pragma once

#include "CoreMinimal.h"


#include "CLRecipeBPFLib.h"
#include "CLItemBPFLib.h"
#include "CLSchematicBPFLib.h"

#include "FGRecipe.h"
#include "FGResearchTreeNode.h"
#include "FGSchematic.h"
#include "Resources/FGItemDescriptor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "Unlocks/FGUnlockSchematic.h"

#include "ContentLibSubsystem.generated.h"


class UFGResearchTree;
class UFGItemCategory;

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FFactoryGame_ProductBuildingCost
{
	GENERATED_BODY()
	FFactoryGame_ProductBuildingCost();
	FFactoryGame_ProductBuildingCost(TSubclassOf<UFGRecipe> InRecipe,TSubclassOf<UObject> InBuilding);

	float GetMjCost() const;

	float GetMjCostForPotential(float Potential) const;
	
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UFGRecipe> Recipe;
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UObject> Building;
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FFactoryGame_RecipeMJ
{
	
	GENERATED_BODY()
	friend struct FFactoryGame_Recipe;
	friend class UContentLibSubsystem;
	FFactoryGame_RecipeMJ();
	FFactoryGame_RecipeMJ(TSubclassOf<UFGRecipe>  Outer);


	bool HasAssignedMJ() const;
	bool TryAssignMJ(UContentLibSubsystem * System);

	int32 GetItemAmount(TSubclassOf<UFGItemDescriptor> Item, bool Ingredient);
	private:
	bool CanCalculateMj(UContentLibSubsystem * System) const;
	float GetAverageBuildingCost(TArray<TSubclassOf<UObject>> Exclude)const;
	void AddValue(const float Value);
	public:
	float GetProductMjValue(TSubclassOf<UFGItemDescriptor> Item,bool PerItem = true , TSubclassOf<UObject> Buildable = nullptr, bool ExcludeManual = true , float Potential = 1.f);

	UPROPERTY(BlueprintReadOnly)
	float MJValueOverride= -1.f;

	UPROPERTY(BlueprintReadOnly)
	float MJ_Average = 0.f;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UFGRecipe> nRecipe;
};




USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FFactoryGame_Descriptor
{
	GENERATED_BODY()

	friend class UContentLibSubsystem;
	friend class UFGItemDescriptor;
	FFactoryGame_Descriptor();
	FFactoryGame_Descriptor(TSubclassOf< class UFGItemDescriptor > InClass);
	FFactoryGame_Descriptor(TSubclassOf< class UFGItemDescriptor > InClass,TSubclassOf< class UFGRecipe > Recipe);
	float GetMj(FFactoryGame_Recipe Recipe,TSubclassOf<UObject> Buildable = nullptr ) const;


	bool HasMj() const;
	float AssignAverageMj(UContentLibSubsystem * System,TArray<TSubclassOf<UFGRecipe>> Exclude = TArray<TSubclassOf<UFGRecipe>>(),TArray<TSubclassOf<UObject>> ExcludeBuilding = TArray<TSubclassOf<UObject>>());
	
	void AssignResourceValue();

	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<class UFGRecipe>> ProductInRecipe;
	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<class UFGRecipe>> IngredientInRecipe;
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf< class UFGItemDescriptor > ItemClass;

	void SetMj(float Value,bool Override = false);

	UPROPERTY(BlueprintReadOnly)
	float MJValue = -1.f;

	~FFactoryGame_Descriptor() = default;

};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FFactoryGame_Schematic
{
	GENERATED_BODY()
	public:
	FFactoryGame_Schematic();
	FFactoryGame_Schematic(TSubclassOf< class UFGSchematic > inClass, UContentLibSubsystem* System);

	void DiscoverUnlocks(UContentLibSubsystem* System);
	
	private:
	void GatherDependencies();
	public:
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf< class UFGSchematic > nClass;

	
	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class UFGSchematic>> nUnlockedBy;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class UFGSchematic>> nDependsOn;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class UFGSchematic>> nDependingOnThis;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class UFGSchematic>> nVisibilityDepOn;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class UFGSchematic>> nVisibilityDepOnThis; 
	UPROPERTY(BlueprintReadWrite)
	int32 GuessedTier = 0;


	~FFactoryGame_Schematic() = default;
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FFactoryGame_Recipe
{
	GENERATED_BODY()

	FFactoryGame_Recipe();
	FFactoryGame_Recipe(const TSubclassOf<UFGRecipe> Class, FFactoryGame_Schematic Schematic);

	void DiscoverMachines(UContentLibSubsystem* System ) const;
	void DiscoverItem(UContentLibSubsystem* System ) const;
	bool IsManualOnly() const;
	bool IsManual() const;

	TArray<float> GetIngredientsForProductRatio(TSubclassOf<UFGItemDescriptor> Item) const;
	float GetItemToTotalProductRatio(TSubclassOf<UFGItemDescriptor> Item,UContentLibSubsystem* System ) const;

	bool UnlockedFromAlternate();
	bool IsBuildGunRecipe() const;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class UFGSchematic>> nUnlockedBy;
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf< class UFGRecipe > nRecipeClass;

	TArray<TSubclassOf<class UFGItemDescriptor>> Products() const;

	TArray<TSubclassOf<class UFGItemDescriptor>> Ingredients() const;

	TArray<TSubclassOf<class UFGItemCategory>> ProductCats() const;

	TArray<TSubclassOf<class UFGItemCategory>> IngredientCats() const;

	UPROPERTY(BlueprintReadOnly)
	FFactoryGame_RecipeMJ MJ;

	UPROPERTY(BlueprintReadOnly)
	FFactoryGame_RecipeMJ MJ_Manual;

	~FFactoryGame_Recipe() = default;

};


/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CONTENTLIB_API UContentLibSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:

	// Called when Content Registration has Finished
	UFUNCTION(BlueprintCallable)
		void ClientInit();
	
	UFUNCTION(BlueprintCallable)
        void FillLoadedClasses();

	UFUNCTION(BlueprintCallable)
        void CollectVisualKits();

	UFUNCTION(BlueprintCallable)
		void FullRecipeCalculation();

	
	UFUNCTION(BlueprintImplementableEvent)
    FFactoryGame_Schematic HandleResearchTreeSchematic(TSubclassOf<UFGSchematic> Schematic, TSubclassOf<UFGResearchTree> Array );

	UFUNCTION(BlueprintImplementableEvent)
	void HandleResearchTreeNodeChange(UFGResearchTreeNode * Node, FContentLib_ResearchNode NodeStruct, TSubclassOf<UFGSchematic> Schematic,TSubclassOf<UFGResearchTree> ResearchTree);

	UFUNCTION(BlueprintCallable)
    FFactoryGame_Schematic HandleSchematic(TSubclassOf<UFGSchematic> Schematic);

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	TMap<TSubclassOf<class AFGBuildable>,TSubclassOf<class UFGBuildingDescriptor>> BuildGunBuildings;

	UPROPERTY(BlueprintReadWrite, Category = "Info")
	TMap<TSubclassOf<class UFGItemDescriptor>, FFactoryGame_Descriptor > Items;

	UPROPERTY(BlueprintReadWrite, Category = "Info")
	TMap<TSubclassOf<class UFGRecipe>, FFactoryGame_Recipe > Recipes;
	
	UPROPERTY(BlueprintReadWrite, Category = "Info")
	TMap<TSubclassOf<class UFGSchematic>, FFactoryGame_Schematic> Schematics;

	UPROPERTY(BlueprintReadWrite, Category = "Info")
	TMap< TSubclassOf<class UFGSchematic>, TSubclassOf<class UFGResearchTree>> SchematicResearchTreeParents;

	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mItems;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mSchematics;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mRecipes;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mItemCategories;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mSchematicCategories;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mBuilders;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mCraftingComps;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> mResearchTrees;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FContentLib_VisualKit> VisualKits;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FContentLib_VisualKit> ImportedVisualKits;

	UPROPERTY(BlueprintReadWrite)
	TMap<TSubclassOf<UFGItemDescriptor>,FString> CreatedItems;
	UPROPERTY(BlueprintReadWrite)
	TArray<FContentLib_ItemPatch> ItemPatches;

	UPROPERTY(BlueprintReadOnly)
	TMap<TSubclassOf<UFGRecipe>,FString> CreatedRecipes;
	UPROPERTY(BlueprintReadOnly)
	TArray<FJsonRecipe> RecipePatches;

	UPROPERTY(BlueprintReadOnly)
	TMap<TSubclassOf<UFGSchematic>,FString> CreatedSchematics;

	UPROPERTY(BlueprintReadOnly)
	TArray<FJsonSchematic> SchematicPatches;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UTexture2D* > Icons;
	
};
