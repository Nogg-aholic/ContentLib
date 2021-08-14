


#include "ContentLibSubsystem.h"



#include "AssetRegistryModule.h"
#include "BPFContentLib.h"
#include "CLUtilBPFLib.h"
#include "FGBuildCategory.h"
#include "FGItemCategory.h"
#include "FGSchematic.h"
#include "FGSchematicCategory.h"
#include "FGWorkBench.h"
#include "Buildables/FGBuildableFactory.h"
#include "Resources/FGItemDescriptor.h"
#include "AvailabilityDependencies/FGSchematicPurchasedDependency.h"
#include "Equipment/FGBuildGun.h"
#include "Reflection/ReflectionHelper.h"
#include "Registry/ModContentRegistry.h"
#include "Resources/FGBuildDescriptor.h"
#include "Resources/FGConsumableDescriptor.h"
#include "Resources/FGItemDescriptorBiomass.h"
#include "Resources/FGItemDescriptorNuclearFuel.h"
#include "Resources/FGResourceDescriptor.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "Unlocks/FGUnlockSchematic.h"


void UContentLibSubsystem::FillLoadedClasses()
{
	mItems.Empty();
	mItemCategories.Empty();
	mBuilders.Empty();
	mCraftingComps.Empty();
	mSchematics.Empty();
	mRecipes.Empty();
	mResearchTrees.Empty();
	mSchematicCategories.Empty();
	GetDerivedClasses(UFGItemDescriptor::StaticClass(), mItems, true);
	GetDerivedClasses(UFGItemCategory::StaticClass(), mItemCategories, true);
	GetDerivedClasses(AFGBuildableFactory::StaticClass(), mBuilders, true);
	GetDerivedClasses(UFGWorkBench::StaticClass(), mCraftingComps, true);
	GetDerivedClasses(UFGSchematic::StaticClass(), mSchematics, true);
	GetDerivedClasses(UFGRecipe::StaticClass(), mRecipes, true);
	GetDerivedClasses(UFGResearchTree::StaticClass(), mResearchTrees, true);
	GetDerivedClasses(UFGSchematicCategory::StaticClass(), mSchematicCategories, true);
	GetDerivedClasses(UFGBuildCategory::StaticClass(), mItemCategories, true);
}

void UContentLibSubsystem::CollectVisualKits()
{
	
	for (const auto ItemPair : Items)
	{
		UFGItemDescriptor* Item = Cast<UFGItemDescriptor>(ItemPair.Key->GetDefaultObject());;
		FContentLib_VisualKit Kit;
		Kit.Mesh = Item->mConveyorMesh->GetPathName();
		Kit.BigIcon = Item->mPersistentBigIcon->GetPathName();
		Kit.SmallIcon = Item->mSmallIcon->GetPathName();
		Kit.FluidColor = Item->mFluidColor;
		Kit.GasColor = Item->mGasColor;
		VisualKits.Add(ItemPair.Key->GetName(), Kit);
	}
}




FFactoryGame_ProductBuildingCost::FFactoryGame_ProductBuildingCost()
{
}

FFactoryGame_ProductBuildingCost::FFactoryGame_ProductBuildingCost(TSubclassOf<UFGRecipe> InRecipe, TSubclassOf<UObject> InBuilding)
{
	Recipe = InRecipe;
	Building = InBuilding;
}

float FFactoryGame_ProductBuildingCost::GetMjCost() const
{
	return GetMjCostForPotential(1.f);
}

float FFactoryGame_ProductBuildingCost::GetMjCostForPotential(const float Potential) const
{
	if (Building)
	{
		if (Building->IsChildOf(AFGBuildableFactory::StaticClass()) && Recipe)
		{
			const float Power = Cast<AFGBuildableFactory>(Building.GetDefaultObject())->CalcProducingPowerConsumptionForPotential(Potential);
			const float TimeMod = FMath::Clamp(Potential, Cast<AFGBuildableFactory>(Building.GetDefaultObject())->GetMinPotential(),Cast<AFGBuildableFactory>(Building.GetDefaultObject())->GetMaxPossiblePotential());
			const float Duration = Recipe.GetDefaultObject()->mManufactoringDuration / TimeMod;
			return (Duration * Power);
		}
		else
			return 0.f;
	}
	return 0.f;
}

FFactoryGame_RecipeMJ::FFactoryGame_RecipeMJ()
{
}

FFactoryGame_RecipeMJ::FFactoryGame_RecipeMJ(TSubclassOf<UFGRecipe> Outer): nRecipe(Outer)
{
}

int32 FFactoryGame_RecipeMJ::GetItemAmount(const TSubclassOf<UFGItemDescriptor> Item, bool Ingredient)
{
	TArray<TSubclassOf<class UFGItemDescriptor>> Out;
	TArray<FItemAmount> Arr = Ingredient ? nRecipe.GetDefaultObject()->GetIngredients(): nRecipe.GetDefaultObject()->GetProducts();
	for (auto i : Arr)
	{
		Out.Add(i.ItemClass);
	}
	if (!Out.Contains(Item))
	{
		UE_LOG(LogTemp, Error, TEXT("Item not part of this Recipe ! "));
		return 0.f;
	}
	return Arr[Out.Find(Item)].Amount;
}

bool FFactoryGame_RecipeMJ::CanCalculateMj(UContentLibSubsystem* System) const
{
	if (!System || !nRecipe)
		return false;
	
	for (auto i : nRecipe.GetDefaultObject()->mIngredients)
	{
		if (!System->Items.Find(i.ItemClass)->HasMj())
			return false;
	}
	return true;
}

bool FFactoryGame_RecipeMJ::HasAssignedMJ() const
{
	if (MJ_Average != 0)
		return true;
	
	return false;
}

void FFactoryGame_RecipeMJ::AddValue(const float Value)
{
	MJ_Average += Value;
	if (Value != MJ_Average)
		MJ_Average /= 2;
}


bool FFactoryGame_RecipeMJ::TryAssignMJ(UContentLibSubsystem* System)
{
	if(!CanCalculateMj(System))
		return false;
	FFactoryGame_Recipe & Recipe = *System->Recipes.Find(nRecipe);
	float Sum = 0.f;
	for (auto Ingredient : nRecipe.GetDefaultObject()->mIngredients)
	{
		System->Items.Find(Ingredient.ItemClass)->AssignAverageMj(System);
		if (System->Items.Find(Ingredient.ItemClass)->HasMj())
		{
			Sum += System->Items.Find(Ingredient.ItemClass)->MJValue * Ingredient.Amount;
		}
		else
			return false;
	}
	Recipe.MJ.AddValue(Sum);
	for (auto Product : Recipe.Products())
	{
		System->Items.Find(Product)->AssignAverageMj(System);
	}
	return true;
	
}

float FFactoryGame_RecipeMJ::GetAverageBuildingCost(const TArray<TSubclassOf<UObject>> Exclude) const
{
	float CostSum = 0.f;
	int32 CostSumDivider = 0;
	TArray<TSubclassOf<UObject>> Producers;
	nRecipe.GetDefaultObject()->GetProducedIn(Producers);
	for(auto & Producer: Producers)
	{
		if(Exclude.Contains(Producer))
			continue;
		FFactoryGame_ProductBuildingCost e = FFactoryGame_ProductBuildingCost(nRecipe,Producer);
		const float TempCost = e.GetMjCost();
		if(TempCost > 0)
		{
			CostSum += TempCost;
			CostSumDivider++;
		}
	}
	if(CostSum > 0 && CostSumDivider > 0)
	{
		return (CostSum / CostSumDivider);
	}
	return CostSum;
}


// TODO :: Product TO MJ Ratio 
float FFactoryGame_RecipeMJ::GetProductMjValue(TSubclassOf<UFGItemDescriptor> Item,bool PerItem  , TSubclassOf<UObject> Buildable, bool ExcludeManual, float Potential)
{
	const int32 ItemAmount = GetItemAmount(Item,false);
	if (ItemAmount == 0 || Potential < 0 )
		return 0.f;

	TArray<TSubclassOf<UObject>> Producers;
	nRecipe.GetDefaultObject()->GetProducedIn(Producers);
	float BuildingCost = 0.f;
	if(!Buildable)
	{
		TArray<TSubclassOf<UObject>> Excludes;
		if(ExcludeManual)
			for(auto i : Producers)
				if (i->IsChildOf(UFGWorkBench::StaticClass()) || i->IsChildOf(AFGBuildGun::StaticClass()))
					Excludes.Add(i);
	
		BuildingCost = GetAverageBuildingCost(Excludes);
	}
	else
	{
		if (!Producers.Contains(Buildable))
		{
			UE_LOG(LogTemp, Error, TEXT("Building cannot Craft this !"));
		}
		else
		{
			const FFactoryGame_ProductBuildingCost Prod = FFactoryGame_ProductBuildingCost(nRecipe,Buildable);
			BuildingCost = Prod.GetMjCostForPotential(Potential);
		}
	}
	if(PerItem)
		return (MJ_Average + BuildingCost) / ItemAmount;
	
	return (MJ_Average + BuildingCost);
}

FFactoryGame_Descriptor::FFactoryGame_Descriptor()
{
}

FFactoryGame_Descriptor::FFactoryGame_Descriptor(TSubclassOf<UFGItemDescriptor> InClass)
{
	ItemClass = InClass;
	AssignResourceValue();
}

FFactoryGame_Descriptor::FFactoryGame_Descriptor(TSubclassOf<UFGItemDescriptor> InClass, TSubclassOf<UFGRecipe> Recipe)
{
	IngredientInRecipe.Add(Recipe);
	ItemClass = InClass;
	AssignResourceValue();
}

float FFactoryGame_Descriptor::GetMj(FFactoryGame_Recipe Recipe,TSubclassOf<UObject> Buildable) const
{
	if (!ItemClass)
		return 0.f;
	if(Recipe.nRecipeClass)
	{
		return Recipe.MJ.GetProductMjValue(ItemClass,true,Buildable);
	}
	return MJValue;
}

void FFactoryGame_Descriptor::AssignResourceValue()
{
	auto& ProcessedItemStruct = *this;
	if (ItemClass->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreIron"))
		{
			ProcessedItemStruct.SetMj(10.76f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreCopper"))
		{
			ProcessedItemStruct.SetMj(11.62f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreBauxite"))
		{
			ProcessedItemStruct.SetMj(11.17f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreGold"))
		{
			ProcessedItemStruct.SetMj(8.49f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_LiquidOil"))
		{
			ProcessedItemStruct.SetMj(.03332f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_Stone"))
		{
			ProcessedItemStruct.SetMj(10.7f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_NitrogenGas"))
		{
			ProcessedItemStruct.SetMj(.01949f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_RawQuartz"))
		{
			ProcessedItemStruct.SetMj(10.03f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_Sulfur"))
		{
			ProcessedItemStruct.SetMj(10.84f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreUranium"))
		{
			ProcessedItemStruct.SetMj(14.85f);
		}
		else if (ItemClass.GetDefaultObject()->mForm == EResourceForm::RF_SOLID && !ItemClass.GetDefaultObject()->GetName().Contains(
            "Desc_CrystalShard"))
            	ProcessedItemStruct.SetMj(10.f);
		else if (ItemClass.GetDefaultObject()->mForm == EResourceForm::RF_LIQUID || ItemClass.GetDefaultObject()->mForm ==
            EResourceForm::RF_GAS)
            	ProcessedItemStruct.SetMj(.01f);

	}
	else if (ItemClass->IsChildOf(UFGItemDescriptorBiomass::StaticClass()))
	{
		ProcessedItemStruct.SetMj(ItemClass.GetDefaultObject()->mEnergyValue);
	}
	else if(ItemClass->IsChildOf(UFGConsumableDescriptor::StaticClass()))
	{
		ProcessedItemStruct.SetMj(10.f);
	}
	else if(ItemClass->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass()))
	{
		//Cast<UFGItemDescriptorNuclearFuel>(ItemClass.GetDefaultObject())->GetSpentFuelClass()
	}
}


void FFactoryGame_Descriptor::SetMj(const float Value, bool Override)
{
	MJValue = Value;
}

bool FFactoryGame_Descriptor::HasMj() const
{
	if (!ItemClass)
		return false;

	return MJValue != -1;
}

float FFactoryGame_Descriptor::AssignAverageMj(UContentLibSubsystem* System, const TArray<TSubclassOf<UFGRecipe>> Exclude, const TArray<TSubclassOf<UObject>> ExcludeBuilding)
{

	if (ItemClass->IsChildOf(UFGResourceDescriptor::StaticClass()))
	{
		AssignResourceValue();
		return MJValue;
	}
	else
	{
		float IngredientCost = 0.f;
		int32 Count = 0;
		for (auto Recipe : ProductInRecipe)
		{
			if(Exclude.Contains(Recipe))
				continue;
			
			auto& RecipeStruct = *System->Recipes.Find(Recipe);
			if (RecipeStruct.MJ.HasAssignedMJ())
			{
				IngredientCost += RecipeStruct.MJ.GetProductMjValue(ItemClass)* RecipeStruct.GetItemToTotalProductRatio(ItemClass,System);
				Count++;
				//UE_LOG(LogTemp, Error, TEXT("%s in Recipe %s IngredientCost-> %f"), *ItemClass->GetName(),*i->GetName(),IngredientCost /Count);
			}
		}
		if (Count > 0 && IngredientCost > 0)
		{
			MJValue = (IngredientCost / Count);
		}
		return MJValue;
	}
	
};











void UContentLibSubsystem::FullRecipeCalculation()
{
	TArray<TSubclassOf<UFGRecipe>> NormalRecipes;
	TArray<TSubclassOf<UFGRecipe>> AlternateRecipes;
	TArray<TSubclassOf<UFGRecipe>> BuildingRecipes;
	TArray<TSubclassOf<UFGRecipe>> ManualOnly;

	for(auto i : Recipes)
	{
		if(i.Value.IsBuildGunRecipe())
			BuildingRecipes.Add(i.Key);
		else if(i.Value.IsManualOnly())
			ManualOnly.Add(i.Key);
		else if(i.Value.UnlockedFromAlternate())
			AlternateRecipes.Add(i.Key);
		else
		{
			NormalRecipes.Add(i.Key);
		}
	}

	// do a bunch of times to average them out
	for(int32 i = 0 ; i < 10; i++)
		UCLUtilBPFLib::CalculateCost(NormalRecipes,this);
	// once for generating , another time to average
	for(int32 i = 0 ; i < 2; i++)
	UCLUtilBPFLib::CalculateCost(AlternateRecipes,this);
	UCLUtilBPFLib::CalculateCost(ManualOnly,this);
	UCLUtilBPFLib::CalculateCost(BuildingRecipes,this);
	
	UE_LOG(LogTemp, Display,TEXT("-----------------------------------------------------"));
	UE_LOG(LogTemp, Display,TEXT("___________________Results Recipes___________________"));

	UCLUtilBPFLib::PrintSortedRecipes(this);
	UE_LOG(LogTemp, Display,TEXT("_____________________________________________________"));
	UE_LOG(LogTemp, Display,TEXT("-----------------------------------------------------"));
	UE_LOG(LogTemp, Display,TEXT("____________________Results Items____________________"));

	UCLUtilBPFLib::PrintSortedItems(this);
	UE_LOG(LogTemp, Display,TEXT("_____________________________________________________"));
	UE_LOG(LogTemp, Display,TEXT("-----------------------------------------------------"));
}

void UContentLibSubsystem::ClientInit()
{

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray< FString> Paths;
	AssetRegistryModule.Get().GetSubPaths(TEXT("/"),Paths , false);
	for (auto i : Paths)
	{
		if (!i.Equals("/Engine", ESearchCase::IgnoreCase) && !i.Equals("/Game", ESearchCase::IgnoreCase) && !i.Equals("/Wwise", ESearchCase::IgnoreCase) && !i.Equals("/Paper2D", ESearchCase::IgnoreCase) && !i.Equals("/Niagara", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogTemp, Warning, TEXT("Loading Mod Assets : %s"), *i);
			TArray<FAssetData> AssetsData;
			FARFilter Filter;
			Filter.TagsAndValues.Add(TEXT("NativeParentClass"));
			Filter.ClassNames.Add("Blueprint");
			Filter.bRecursivePaths = true;
			Filter.PackagePaths.Add(*i);
			Filter.bIncludeOnlyOnDiskAssets = true;
			AssetRegistryModule.Get().GetAssets(Filter, AssetsData);
			for (auto e : AssetsData)
			{
				if (!e.IsAssetLoaded())
				{
					FString NativeParentPath = *e.TagsAndValues.Find("NativeParentClass");
					UClass* Parent = FindObject<UClass>(NULL, *NativeParentPath);
					if (Parent &&
						Parent->IsChildOf(UFGItemDescriptor::StaticClass()) 
						|| Parent->IsChildOf(UFGSchematic::StaticClass()) 
						|| Parent->IsChildOf(UFGResearchTree::StaticClass()) 
						|| Parent->IsChildOf(UFGItemCategory::StaticClass())
						|| Parent->IsChildOf(UFGRecipe::StaticClass())
						|| Parent->IsChildOf(UFGSchematicCategory::StaticClass())
						|| Parent->IsChildOf(UFGBuildCategory::StaticClass())
						)
					{
						LoadObject<UClass>(NULL, *e.ObjectPath.ToString().Append("_C"));
					}
				}
			}
		}
	}
	
	TArray<FAssetData> AssetsData;
	FARFilter Filter;
	Filter.TagsAndValues.Add(TEXT("NativeParentClass"));
	Filter.ClassNames.Add("Blueprint");
	Filter.PackagePaths.Add("/Game/FactoryGame/Recipes");
	Filter.PackagePaths.Add("/Game/FactoryGame/Schematics");
	Filter.PackagePaths.Add("/Game/FactoryGame/Resources");
	Filter.bRecursivePaths = true;
	Filter.bIncludeOnlyOnDiskAssets = true;
	AssetRegistryModule.Get().GetAssets(Filter, AssetsData);
	for(auto i : AssetsData)
	{
		if (!i.IsAssetLoaded())
		{
			FString NativeParentPath = *i.TagsAndValues.Find("NativeParentClass");
			UClass* Parent = FindObject<UClass>(NULL, *NativeParentPath);
			if (Parent &&
				Parent->IsChildOf(UFGItemDescriptor::StaticClass())
				|| Parent->IsChildOf(UFGSchematic::StaticClass())
				|| Parent->IsChildOf(UFGResearchTree::StaticClass())
				|| Parent->IsChildOf(UFGItemCategory::StaticClass())
				|| Parent->IsChildOf(UFGRecipe::StaticClass())
				)
			{
				LoadObject<UClass>(NULL, *i.ObjectPath.ToString().Append("_C"));
			}
		}
	}

	
	TArray<TSubclassOf<class UFGSchematic>> toProcess;
	TArray<UClass*> Arr;
	GetDerivedClasses(UFGSchematic::StaticClass(), Arr, true);
	for(auto i : Arr)
	{
		HandleSchematic(i);
	}

	static FStructProperty* NodeDataStructProperty = nullptr;
	static FClassProperty* SchematicStructProperty = nullptr;
	static UClass* ResearchTreeNodeClass = nullptr;

	ResearchTreeNodeClass = LoadClass<UFGResearchTreeNode>(
        nullptr, TEXT("/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"));
	if (ResearchTreeNodeClass)
	{
		TArray<UClass*> Trees;
		GetDerivedClasses(UFGResearchTree::StaticClass(), Arr, true);
		for(auto i : Trees)
		{
			NodeDataStructProperty = FReflectionHelper::FindPropertyChecked<FStructProperty>(
            ResearchTreeNodeClass, TEXT("mNodeDataStruct"));
			SchematicStructProperty = FReflectionHelper::FindPropertyByShortNameChecked<FClassProperty>(
                NodeDataStructProperty->Struct, TEXT("Schematic"));
			const TArray<UFGResearchTreeNode*> Nodes = UFGResearchTree::GetNodes(i);
			for (UFGResearchTreeNode* Node : Nodes)
			{
				if (!Node)
					continue;
				TSubclassOf<UFGSchematic> Schematic = Cast<UClass>(
                    SchematicStructProperty->GetPropertyValue_InContainer(
                        NodeDataStructProperty->ContainerPtrToValuePtr<void>(Node)));
				if (!Schematic)
					continue;
				Schematics.Add(Schematic, HandleResearchTreeSchematic(Schematic, i));
			}
		}	
	}
	
}



FFactoryGame_Schematic UContentLibSubsystem::HandleSchematic(const TSubclassOf<class UFGSchematic> Schematic)
{
	return Schematics.Add(Schematic, FFactoryGame_Schematic(Schematic, this));
}



FFactoryGame_Recipe::FFactoryGame_Recipe()
{
	nRecipeClass = nullptr;
	nUnlockedBy = {};
};

FFactoryGame_Recipe::FFactoryGame_Recipe(const TSubclassOf<UFGRecipe> Class, const FFactoryGame_Schematic Schematic)
{
	nRecipeClass = Class;
	nUnlockedBy.Add(Schematic.nClass);
	MJ = FFactoryGame_RecipeMJ(Class);
}

TArray<float> FFactoryGame_Recipe::GetIngredientsForProductRatio(const TSubclassOf<UFGItemDescriptor> Item) const
{
	const auto CDO = nRecipeClass.GetDefaultObject();
	TArray<float> Array;
	for (const auto i : CDO->mIngredients)
	{
		if(i.ItemClass != Item)
			continue;
		for (const auto e : CDO->mProduct)
		{
			Array.Add(e.Amount / i.Amount);
		}
	}
	return Array;
}

float FFactoryGame_Recipe::GetItemToTotalProductRatio(TSubclassOf<UFGItemDescriptor> Item, UContentLibSubsystem* System ) const
{
	if (!System)
	{
		UE_LOG(LogTemp, Error, TEXT("------------------------FFactoryGame_Recipe nullptr Subsystem in function GetItemToTotalProductRatio ----------------------"));
		return 0.f;
	}
	const auto CDO = nRecipeClass.GetDefaultObject();
	if(CDO->mProduct.Num() > 1)
	{
		if(MJ.HasAssignedMJ())
		{
			for (const auto Product : CDO->mProduct)
			{
				if(Product.ItemClass != Item)
					continue;
				const FFactoryGame_Descriptor & ProductStruct = *System->Items.Find(Product.ItemClass);
				if(ProductStruct.HasMj())
					return FMath::Clamp((ProductStruct.MJValue * Product.Amount) / MJ.MJ_Average,0.f,1.f);
				else
					return 1.f;
			}
			// 10000 Cost -> product is 300 * 10 -> 3000/10000 -> 0.3
		}
	}
	return 1.f;
}

void FFactoryGame_Recipe::DiscoverMachines(UContentLibSubsystem* System ) const
{
	if (!System)
	{
		UE_LOG(LogTemp, Error, TEXT("------------------------FFactoryGame_Recipe nullptr Subsystem in function DiscoverMachines ----------------------"));
		return;
	}
	TArray<TSubclassOf<UObject>> BuildClasses;
	nRecipeClass.GetDefaultObject()->GetProducedIn(BuildClasses);
	BuildClasses.Remove(nullptr);
	if (Products().IsValidIndex(0))
	{
		for (auto j : BuildClasses)
		{
			TArray<UClass*> Arr;
			GetDerivedClasses(j, Arr,true);
			if(!Arr.Contains(j) && !j->IsNative())
			{
				Arr.Add(j);
			}
			for(auto h : Arr)
			{
				if (h->IsChildOf(AFGBuildGun::StaticClass()) && Products()[0]->IsChildOf(UFGBuildDescriptor::StaticClass()))
				{
					TSubclassOf<UFGBuildDescriptor> Desc = *Products()[0];
					TSubclassOf<AFGBuildable> Buildable;
					Buildable = Desc.GetDefaultObject()->GetBuildClass(Desc);
					if (!System->BuildGunBuildings.Contains(*Desc))
						System->BuildGunBuildings.Add(Buildable, *Desc);
				}
			}
		}
	}
}

void FFactoryGame_Recipe::DiscoverItem(UContentLibSubsystem* System ) const
{
	if (!System)
	{
		UE_LOG(LogTemp, Error, TEXT("------------------------FFactoryGame_Recipe nullptr Subsystem in function DiscoverItem ----------------------"));
		return;
	}
	for (auto Ingredient : Ingredients())
	{
		FFactoryGame_Descriptor Item = FFactoryGame_Descriptor(Ingredient, nRecipeClass);
		if (System->Items.Contains(Ingredient))
			System->Items.Find(Ingredient)->IngredientInRecipe.Add(nRecipeClass);
		else
			System->Items.Add(Ingredient, Item);
	}

	for (auto Product : Products())
	{
		FFactoryGame_Descriptor Item = FFactoryGame_Descriptor(Product);
		if (System->Items.Contains(Product))
			System->Items.Find(Product)->ProductInRecipe.Add(nRecipeClass);
		else
		{
			Item.ProductInRecipe.Add(nRecipeClass);
			System->Items.Add(Product, Item);
		}
	}
}

bool FFactoryGame_Recipe::IsManualOnly() const
{
	if(!IsManual())
		return false;
	
	TArray<TSubclassOf<UObject>> Producers;
	nRecipeClass.GetDefaultObject()->GetProducedIn(Producers);
	for(auto Producer : Producers)
		if(Producer && !Producer->IsChildOf(UFGWorkBench::StaticClass()))
			return false;
	return true;
}

bool FFactoryGame_Recipe::IsManual() const
{
	TArray<TSubclassOf<UObject>> Producers;
	nRecipeClass.GetDefaultObject()->GetProducedIn(Producers);
	for(auto Producer : Producers)
		if(Producer->IsChildOf(UFGWorkBench::StaticClass()))
			return true;

	return false;
}
bool FFactoryGame_Recipe::UnlockedFromAlternate()
{
	for(auto Schematic : nUnlockedBy)
		if(Schematic.GetDefaultObject()->mType == ESchematicType::EST_Alternate)
			return true;

	return false;
}
bool FFactoryGame_Recipe::IsBuildGunRecipe() const
{
	TArray<TSubclassOf<UObject>> Producers;
	nRecipeClass.GetDefaultObject()->GetProducedIn(Producers);
	for(auto Producer : Producers)
		if(Producer->IsChildOf(AFGBuildGun::StaticClass()))
			return true;

	return false;
}
TArray<TSubclassOf<UFGItemDescriptor>> FFactoryGame_Recipe::Products() const
{
	TArray<TSubclassOf<class UFGItemDescriptor>> out;
	TArray<FItemAmount> ProductStructs = nRecipeClass.GetDefaultObject()->GetProducts();
	for (auto ProductStruct : ProductStructs)
	{
		out.Add(ProductStruct.ItemClass);
	}
	return out;
}

TArray<TSubclassOf<UFGItemDescriptor>> FFactoryGame_Recipe::Ingredients() const
{
	TArray<TSubclassOf<class UFGItemDescriptor>> out;
	TArray<FItemAmount> IngredientStructs = nRecipeClass.GetDefaultObject()->GetIngredients();
	for (auto IngredientStruct : IngredientStructs)
	{
		out.Add(IngredientStruct.ItemClass);
	}
	return out;
}

TArray<TSubclassOf<UFGItemCategory>> FFactoryGame_Recipe::ProductCats() const
{
	TArray<TSubclassOf<class UFGItemCategory>> Out;
	TArray<FItemAmount> ProductStructs = nRecipeClass.GetDefaultObject()->GetProducts();
	for (auto ProductStruct : ProductStructs)
	{
		if (!Out.Contains(ProductStruct.ItemClass.GetDefaultObject()->GetItemCategory(ProductStruct.ItemClass)))
			Out.Add(ProductStruct.ItemClass.GetDefaultObject()->GetItemCategory(ProductStruct.ItemClass));
	}
	return Out;
}

TArray<TSubclassOf<UFGItemCategory>> FFactoryGame_Recipe::IngredientCats() const
{
	TArray<TSubclassOf<class UFGItemCategory>> Out;
	TArray<FItemAmount> IngredientStructs = nRecipeClass.GetDefaultObject()->GetIngredients();
	for (auto Ingredient : IngredientStructs)
	{
		if (!Out.Contains(Ingredient.ItemClass.GetDefaultObject()->GetItemCategory(Ingredient.ItemClass)))
			Out.Add(Ingredient.ItemClass.GetDefaultObject()->GetItemCategory(Ingredient.ItemClass));
	}
	return Out;
}

FFactoryGame_Schematic::FFactoryGame_Schematic()
{
}

FFactoryGame_Schematic::FFactoryGame_Schematic(TSubclassOf<UFGSchematic> inClass, UContentLibSubsystem* System)
{
	nClass = inClass;
	DiscoverUnlocks(System);
}

void FFactoryGame_Schematic::DiscoverUnlocks(UContentLibSubsystem* System)
{
	GatherDependencies();

	TArray<UFGUnlock*> Unlocks = nClass.GetDefaultObject()->GetUnlocks(nClass);
	for (auto Unlock : Unlocks)
	{
		// Recipe unlocks make struct for it and save Buildings found
		if (Cast<UFGUnlockRecipe>(Unlock))
		{
			TArray<TSubclassOf<class UFGRecipe>> UnlockRecipes = Cast<UFGUnlockRecipe>(Unlock)->GetRecipesToUnlock();

			for (auto UnlockRecipe : UnlockRecipes)
			{
				if (!UnlockRecipe)
					continue;

				if (!System->Recipes.Contains(UnlockRecipe))
				{
					FFactoryGame_Recipe RecipeStruct = FFactoryGame_Recipe(UnlockRecipe, *this);
					RecipeStruct.DiscoverItem(System);
					RecipeStruct.DiscoverMachines(System);
					System->Recipes.Add(UnlockRecipe, RecipeStruct);
				}
				else
				{
					System->Recipes.Find(UnlockRecipe)->nUnlockedBy.Add(nClass);
				}
			}
		} // schematics unlocks cause recursion
		else if (Cast<UFGUnlockSchematic>(Unlock))
		{
			TArray<TSubclassOf<class UFGSchematic>> UnlockSchematics = Cast<UFGUnlockSchematic>(Unlock)->GetSchematicsToUnlock();

			for (auto UnlockSchematic : UnlockSchematics)
			{
				if (!System->Schematics.Contains(UnlockSchematic))
				{
					System->HandleSchematic(UnlockSchematic);
				}
			}
		}
	}
}

void FFactoryGame_Schematic::GatherDependencies()
{
	TArray<UFGAvailabilityDependency*> Out_SchematicDependencies;
	nClass.GetDefaultObject()->GetSchematicDependencies(nClass, Out_SchematicDependencies);
	for (auto Dependency : Out_SchematicDependencies)
	{
		if (!Dependency)
			continue;
		TArray<TSubclassOf<class UFGSchematic>> Out_Schematics;
		const UFGSchematicPurchasedDependency* Dep = Cast<UFGSchematicPurchasedDependency>(Dependency);
		if (Dep)
		{
			Dep->GetSchematics(Out_Schematics);
			for (auto Schematic : Out_Schematics)
			{
				if (!Schematic)
					continue;
				if (!nDependsOn.Contains(Schematic))
					nDependsOn.Add(Schematic);
			}
		}
	}
}
