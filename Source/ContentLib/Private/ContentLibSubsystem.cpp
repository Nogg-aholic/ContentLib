


#include "ContentLibSubsystem.h"
#include "ContentLib.h"


#include "AssetRegistry/AssetRegistryModule.h"
#include "BPFContentLib.h"
#include "CLUtilBPFLib.h"
#include "FGBuildCategory.h"
#include "FGItemCategory.h"
#include "FGSchematic.h"
#include "FGSchematicCategory.h"
#include "FGWorkBench.h"
#include "Buildables/FGBuildableManufacturer.h"
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
#include "UObject/CoreRedirects.h"
#include "Kismet/KismetSystemLibrary.h"


void UContentLibSubsystem::FillLoadedClasses(bool logBuilders)
{
	mItems.Empty();
	mCategories.Empty();
	mBuilders.Empty();
	mCraftingComps.Empty();
	mSchematics.Empty();
	mRecipes.Empty();
	mResearchTrees.Empty();

	GetDerivedClasses(UFGItemDescriptor::StaticClass(), mItems, true);
	GetDerivedClasses(UFGCategory::StaticClass(), mCategories, true);
	GetDerivedClasses(AFGBuildableManufacturer::StaticClass(), mBuilders, true);
	GetDerivedClasses(UFGWorkBench::StaticClass(), mCraftingComps, true);
	GetDerivedClasses(UFGSchematic::StaticClass(), mSchematics, true);
	GetDerivedClasses(UFGRecipe::StaticClass(), mRecipes, true);
	GetDerivedClasses(UFGResearchTree::StaticClass(), mResearchTrees, true);

	if (logBuilders) {
		UE_LOG(LogContentLib, Display, TEXT("All detected producers (AFGBuildableManufacturer):"));
		for (auto& entry : mBuilders) {
			UE_LOG(LogContentLib, Display, TEXT("'%s' at path %s"), *entry->GetName(), *entry->GetClassPathName().ToString());
		}
		UE_LOG(LogContentLib, Display, TEXT("All detected crafting components (UFGWorkBench):"));
		for (auto& entry : mCraftingComps) {
			UE_LOG(LogContentLib, Display, TEXT("'%s' at path %s"), *entry->GetName(), *entry->GetClassPathName().ToString());
		}
	}
}

void UContentLibSubsystem::CollectVisualKits()
{
	for (const auto& ItemPair : Items) {
		const auto descriptor = ItemPair.Key;
		if (!descriptor) {
			continue;
		}
		// TODO why is this done? Presumably to ensure valid object?
		if (!IsValid(descriptor.GetDefaultObject())) {
			continue;
		}
		FContentLib_VisualKit Kit;
		Kit.Mesh = UFGItemDescriptor::GetItemMesh(descriptor)->GetPathName();
		Kit.BigIcon = UFGItemDescriptor::GetBigIcon(descriptor)->GetPathName();
		Kit.SmallIcon = UFGItemDescriptor::GetSmallIcon(descriptor)->GetPathName();
		Kit.FluidColor = UFGItemDescriptor::GetFluidColor(descriptor);
		Kit.GasColor = UFGItemDescriptor::GetGasColor(descriptor);
		VisualKits.Add(descriptor->GetName(), Kit);
	}

	for (const auto& SchematicPair : Schematics) {
		const auto schematic = SchematicPair.Key;
		if (!schematic) {
			continue;
		}
		// TODO why is this done? Presumably to ensure valid object?
		if (!schematic.GetDefaultObject()) {
			continue;
		}
		FContentLib_VisualKit Kit;
		Kit.Mesh = TEXT("None");
		Kit.SmallIcon = UFGSchematic::GetSmallIcon(schematic)->GetPathName();
		auto brush = UFGSchematic::GetItemIcon(schematic);
		auto brushPathname = brush.GetResourceObject()->GetPathName();
		if (brush.HasUObject()) {
			Kit.BigIcon = brushPathname;
		} else {
			// This happens on a ton of vanilla stuff that isn't meant to have icons... not sure under what conditions it would be useful to log it
			// UE_LOG(LogContentLib, Warning, TEXT("Non-UObject brush detected for schematic %s, using its small icon as its big icon for visual kit. BigIcon resource object is: %s"), *schematic->GetPathName(), *brush.GetResourceObject()->GetPathName());
			Kit.BigIcon = Kit.SmallIcon;
		}
		if (brushPathname.Equals("None") && UFGSchematic::GetType(schematic) == ESchematicType::EST_Milestone) {
			UE_LOG(LogContentLib, Warning, TEXT("Visual kit for Milestone schematic %s has no big icon, this will cause display problems in the HUB"), *schematic->GetPathName());
		}
		Kit.FluidColor = FColor::Magenta;
		Kit.GasColor = FColor::Magenta;
		VisualKits.Add(schematic->GetName(), Kit);
	}
}


FFactoryGame_ProductBuildingCost::FFactoryGame_ProductBuildingCost() {}

FFactoryGame_ProductBuildingCost::FFactoryGame_ProductBuildingCost(TSubclassOf<UFGRecipe> InRecipe, TSubclassOf<UObject> InBuilding)
{
	Recipe = InRecipe;
	Building = InBuilding;
}

float FFactoryGame_ProductBuildingCost::GetMjCost() const {
	return GetMjCostForPotential(1.f);
}

float FFactoryGame_ProductBuildingCost::GetMjCostForPotential(const float Potential) const
{
	if (Building) {
		if (Building->IsChildOf(AFGBuildableFactory::StaticClass()) && Recipe) {
			const float Power = Cast<AFGBuildableFactory>(Building.GetDefaultObject())->CalcProducingPowerConsumptionForPotential(Potential);
			const float TimeMod = FMath::Clamp(Potential, Cast<AFGBuildableFactory>(Building.GetDefaultObject())->GetCurrentMinPotential(),Cast<AFGBuildableFactory>(Building.GetDefaultObject())->GetCurrentMaxPotential());
			const float Duration = UFGRecipe::GetManufacturingDuration(Recipe) / TimeMod;
			return (Duration * Power);
		}
	}
	return 0.f;
}

FFactoryGame_RecipeMJ::FFactoryGame_RecipeMJ(){ }

FFactoryGame_RecipeMJ::FFactoryGame_RecipeMJ(TSubclassOf<UFGRecipe> Outer): nRecipe(Outer){ }

int32 FFactoryGame_RecipeMJ::GetItemAmount(const TSubclassOf<UFGItemDescriptor> Item, bool Ingredient) const
{
	const auto CDO = nRecipe.GetDefaultObject();
	if (!IsValid(CDO)) {
		UE_LOG(LogContentLib, Error, TEXT("GetItemAmount - Invalid nRecipe!"));
		return 0;
	}

	const TArray<FItemAmount>& Arr = Ingredient ? CDO->GetIngredients() : CDO->GetProducts();
	for (auto& i : Arr) {
		if (i.ItemClass == Item) {
			return i.Amount;
		}
	}
	UE_LOG(LogContentLib, Error, TEXT("GetItemAmount - Item not part of this Recipe!"));
	return 0;
}

bool FFactoryGame_RecipeMJ::CanCalculateMj(UContentLibSubsystem* System) const
{
	if (!System || !nRecipe)
		return false;
	
	for (auto& i : UFGRecipe::GetIngredients(nRecipe)) {
		if (!System->Items.Find(i.ItemClass)->HasMj())
			return false;
	}
	return true;
}

bool FFactoryGame_RecipeMJ::HasAssignedMJ() const
{
	return MJ_Average != 0;
}

void FFactoryGame_RecipeMJ::AddValue(const float Value)
{
	MJ_Average += Value;
	if (Value != MJ_Average)
		MJ_Average /= 2;
}


bool FFactoryGame_RecipeMJ::TryAssignMJ(UContentLibSubsystem* System)
{
	if (!CanCalculateMj(System))
		return false;

	FFactoryGame_Recipe & Recipe = *System->Recipes.Find(nRecipe);
	const auto ingredients = UFGRecipe::GetIngredients(nRecipe);
	float Sum = 0.f;
	for (auto& Ingredient : ingredients) {
		if (FFactoryGame_Descriptor* Desc = System->Items.Find(Ingredient.ItemClass)) {
			Desc->AssignAverageMj(System);
			if (Desc->HasMj()) {
				Sum += Desc->MJValue * Ingredient.Amount;
			} else {
				return false;
			}
		}
	}
	Recipe.MJ.AddValue(Sum);
	for (auto& Product : Recipe.Products()) {
		System->Items.Find(Product)->AssignAverageMj(System);
	}
	return true;
	
}

float FFactoryGame_RecipeMJ::GetAverageBuildingCost(const TArray<TSubclassOf<UObject>> Exclude) const
{
	float CostSum = 0.f;
	int32 CostSumDivider = 0;
	for(auto& Producer: UFGRecipe::GetProducedIn(nRecipe)) {
		if(Exclude.Contains(Producer))
			continue;
		FFactoryGame_ProductBuildingCost e = FFactoryGame_ProductBuildingCost(nRecipe,Producer);
		const float TempCost = e.GetMjCost();
		if(TempCost > 0) {
			CostSum += TempCost;
			CostSumDivider++;
		}
	}
	if(CostSum > 0 && CostSumDivider > 0) {
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

	TArray<TSubclassOf<UObject>> Producers = UFGRecipe::GetProducedIn(nRecipe);
	float BuildingCost = 0.f;
	if(!Buildable) {
		TArray<TSubclassOf<UObject>> Excludes;
		if(ExcludeManual)
			for(auto& i : Producers)
				if (i->IsChildOf(UFGWorkBench::StaticClass()) || i->IsChildOf(AFGBuildGun::StaticClass()))
					Excludes.Add(i);
	
		BuildingCost = GetAverageBuildingCost(Excludes);
	}
	else {
		if (!Producers.Contains(Buildable)) {
			UE_LOG(LogContentLib, Error, TEXT("Can't calculate GetProductMjValue: Buildable %s can't produce %s !"), *UKismetSystemLibrary::GetDisplayName(Buildable), *UKismetSystemLibrary::GetDisplayName(nRecipe));
		}
		else {
			const FFactoryGame_ProductBuildingCost Prod = FFactoryGame_ProductBuildingCost(nRecipe,Buildable);
			BuildingCost = Prod.GetMjCostForPotential(Potential);
		}
	}
	if(PerItem)
		return (MJ_Average + BuildingCost) / ItemAmount;
	
	return (MJ_Average + BuildingCost);
}

FFactoryGame_Descriptor::FFactoryGame_Descriptor() { }

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
	if (!ItemClass) {
		return 0.f;
	}
	if (Recipe.nRecipeClass) {
		return Recipe.MJ.GetProductMjValue(ItemClass,true,Buildable);
	}
	return MJValue;
}

void FFactoryGame_Descriptor::AssignResourceValue()
{
	// if i recall correctly, this was just the MJ cost (power usage / miner output ) for producing one unit of that by miner yea,
	// when i came up with going upwards instead of downwards from a recipe to calculate the cost , i needed initial values 
	// it would be better to grab that of the miners that exist
	// but i think there were some other issues in getting a useful value, considering all miners and so on
	// - Nog
	// https://discord.com/channels/@me/677971486400577556/1166779976541012050
	if (!ItemClass) {
		return;
	}
	auto& ProcessedItemStruct = *this;
	if (ItemClass->IsChildOf(UFGResourceDescriptor::StaticClass())) {
		if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreIron")) {
			ProcessedItemStruct.SetMj(10.76f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreCopper")) {
			ProcessedItemStruct.SetMj(11.62f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreBauxite")) {
			ProcessedItemStruct.SetMj(11.17f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreGold")) {
			ProcessedItemStruct.SetMj(8.49f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_LiquidOil")) {
			ProcessedItemStruct.SetMj(.03332f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_Stone")) {
			ProcessedItemStruct.SetMj(10.7f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_NitrogenGas")) {
			ProcessedItemStruct.SetMj(.01949f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_RawQuartz")) {
			ProcessedItemStruct.SetMj(10.03f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_Sulfur")) {
			ProcessedItemStruct.SetMj(10.84f);
		}
		else if (ItemClass.GetDefaultObject()->GetName().Contains("Desc_OreUranium")) {
			ProcessedItemStruct.SetMj(14.85f);
		}
		else if (UFGItemDescriptor::GetForm(ItemClass) == EResourceForm::RF_SOLID && !ItemClass.GetDefaultObject()->GetName().Contains(
            "Desc_CrystalShard"))
            	ProcessedItemStruct.SetMj(10.f);
		else if (UFGItemDescriptor::GetForm(ItemClass) == EResourceForm::RF_LIQUID || UFGItemDescriptor::GetForm(ItemClass) ==
            EResourceForm::RF_GAS)
            	ProcessedItemStruct.SetMj(.01f);

	}
	else if (ItemClass->IsChildOf(UFGItemDescriptorBiomass::StaticClass())) {
		ProcessedItemStruct.SetMj(UFGItemDescriptor::GetEnergyValue(ItemClass));
	}
	else if(ItemClass->IsChildOf(UFGConsumableDescriptor::StaticClass())) {
		ProcessedItemStruct.SetMj(10.f);
	}
	else if(ItemClass->IsChildOf(UFGItemDescriptorNuclearFuel::StaticClass())) {
		// TODO not sure why this was here, it's a no op?
		//Cast<UFGItemDescriptorNuclearFuel>(ItemClass.GetDefaultObject())->GetSpentFuelClass()
	}
}

void FFactoryGame_Descriptor::SetMj(const float Value, bool Override)
{
	MJValue = Value;
}

bool FFactoryGame_Descriptor::HasMj() const
{
	if (!ItemClass) {
		return false;
	}
	return MJValue != -1;
}

float FFactoryGame_Descriptor::AssignAverageMj(UContentLibSubsystem* System, const TArray<TSubclassOf<UFGRecipe>> Exclude, const TArray<TSubclassOf<UObject>> ExcludeBuilding)
{
	if (!ItemClass) {
		return 0.f;
	}
	if (ItemClass->IsChildOf(UFGResourceDescriptor::StaticClass())) {
		AssignResourceValue();
		return MJValue;
	}
	
	
	float IngredientCost = 0.f;
	int32 Count = 0;
	for (auto& Recipe : ProductInRecipe) {
		if(Exclude.Contains(Recipe))
			continue;
		
		auto& RecipeStruct = *System->Recipes.Find(Recipe);
		if (RecipeStruct.MJ.HasAssignedMJ()) {
			IngredientCost += RecipeStruct.MJ.GetProductMjValue(ItemClass)* RecipeStruct.GetItemToTotalProductRatio(ItemClass,System);
			Count++;
			//UE_LOG(LogContentLib, Error, TEXT("%s in Recipe %s IngredientCost-> %f"), *ItemClass->GetName(),*i->GetName(),IngredientCost /Count);
		}
	}
	if (Count > 0 && IngredientCost > 0) {
		MJValue = (IngredientCost / Count);
	}
	return MJValue;
}

void UContentLibSubsystem::FullRecipeCalculation()
{
	TArray<TSubclassOf<UFGRecipe>> NormalRecipes;
	TArray<TSubclassOf<UFGRecipe>> AlternateRecipes;
	TArray<TSubclassOf<UFGRecipe>> BuildingRecipes;
	TArray<TSubclassOf<UFGRecipe>> ManualOnly;

	for(auto& i : Recipes) {
		if(i.Value.IsBuildGunRecipe())
			BuildingRecipes.Add(i.Key);
		else if(i.Value.IsManualOnly())
			ManualOnly.Add(i.Key);
		else if(i.Value.UnlockedFromAlternate())
			AlternateRecipes.Add(i.Key);
		else {
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
	
	UE_LOG(LogContentLib, Display,TEXT("-----------------------------------------------------"));
	UE_LOG(LogContentLib, Display,TEXT("___________________Results Recipes___________________"));

	UCLUtilBPFLib::PrintSortedRecipes(this);
	UE_LOG(LogContentLib, Display,TEXT("_____________________________________________________"));
	UE_LOG(LogContentLib, Display,TEXT("-----------------------------------------------------"));
	UE_LOG(LogContentLib, Display,TEXT("____________________Results Items____________________"));

	UCLUtilBPFLib::PrintSortedItems(this);
	UE_LOG(LogContentLib, Display,TEXT("_____________________________________________________"));
	UE_LOG(LogContentLib, Display,TEXT("-----------------------------------------------------"));
}

void UContentLibSubsystem::ClientInit()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray< FString> Paths;
	UE_LOG(LogContentLib, Warning, TEXT("ContentLib loading relevant Mod Assets..."));
	AssetRegistryModule.Get().GetSubPaths(TEXT("/"),Paths , false);
	for (auto& i : Paths) {
		if (!i.Equals("/Engine", ESearchCase::IgnoreCase)
			&& !i.Equals("/Game", ESearchCase::IgnoreCase)
			&& !i.Equals("/Wwise", ESearchCase::IgnoreCase)
			&& !i.Equals("/Paper2D", ESearchCase::IgnoreCase)
			&& !i.Equals("/Niagara", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogContentLib, Warning, TEXT("ContentLib Loading Assets of Mod: %s"), *i);
			TArray<FAssetData> AssetsData;
			FARFilter Filter;
			Filter.TagsAndValues.Add(TEXT("NativeParentClass"));
			Filter.ClassNames.Add("Blueprint");
			Filter.bRecursivePaths = true;
			Filter.PackagePaths.Add(*i);
			Filter.bIncludeOnlyOnDiskAssets = true;
			AssetRegistryModule.Get().GetAssets(Filter, AssetsData);
			for (const auto& asset : AssetsData) {
				if (!asset.IsAssetLoaded()) {
					FString NativeParentPath = *asset.TagsAndValues.FindTag("NativeParentClass").AsString();
					UClass* Parent = FindObject<UClass>(NULL, *NativeParentPath);
					FString TempPackageName = asset.PackageName.ToString();
					if (!Parent) {
						UE_LOG(LogContentLib, Error, TEXT("Somehow the parent of asset %s is None, report this to that mod's author"), *TempPackageName);
						continue;
					}
					FString TempParentName = Parent->GetPathName();
					UE_LOG(LogContentLibAssetParsing, VeryVerbose, TEXT("Parsing asset %s with parent %s"), *TempPackageName, *TempParentName);
					const FString assetPathString = asset.GetObjectPathString().Append("_C"); // U8 Migrated asset.GetObjectPathString() from asset.ObjectPath.ToString()
					UE_LOG(LogContentLibAssetParsing, VeryVerbose, TEXT("Asset path string is %s"), *assetPathString);
					if (
						   Parent->IsChildOf(UFGItemDescriptor::StaticClass()) 
						|| Parent->IsChildOf(UFGSchematic::StaticClass()) 
						|| Parent->IsChildOf(UFGResearchTree::StaticClass()) 
						|| Parent->IsChildOf(UFGCategory::StaticClass())
						|| Parent->IsChildOf(UFGRecipe::StaticClass())
					) {
						if(Parent == UFGItemDescriptor::StaticClass()) {
							if (DumpItems.Contains(*assetPathString)) {
								// replace Load
								const FString JsonString = *DumpItems.Find(*assetPathString);
								FContentLib_Item Item = UCLItemBPFLib::GenerateCLItemFromString(JsonString);
								FString Left; FString Right; 
								assetPathString.Split("_C", &Left, &Right,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
								Left.Split(".", &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
								TSubclassOf<UObject>  Obj =  UBPFContentLib::CreateContentLibClass(Right,  UFGItemDescriptor::StaticClass());
								// If consolidating blueprints, make sure redirectors are created for the consolidated blueprint class and CDO
								
								// One redirector for the class
								const ECoreRedirectFlags Flags = ECoreRedirectFlags::Type_Package | ECoreRedirectFlags::Option_MatchSubstring;
								TArray<FCoreRedirect> Redirects;
								Redirects.Add(FCoreRedirect(Flags, *asset.AssetName.ToString(), Obj->GetPathName()));
								FCoreRedirects::AddRedirectList(Redirects,Obj->GetName());
								auto* I = LoadObject<UClass>(NULL, *assetPathString);
								if(I != Obj) {
									UE_LOG(LogContentLib,Fatal,TEXT("Redirect Failed for %s"), *assetPathString);
								}
							}
							else
							{
								DumpItems.Add(*assetPathString,UCLItemBPFLib::GenerateFromDescriptorClass(LoadObject<UClass>(NULL, *assetPathString)));
							}
						}
						else {
							LoadObject<UClass>(NULL, *assetPathString);
						}
					}
				}
			}
		}
	}
	UE_LOG(LogContentLib, Warning, TEXT("ContentLib done loading relevant Mod Assets"));
	
	UE_LOG(LogContentLib, Warning, TEXT("ContentLib loading relevant FG Assets..."));
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
	for (FAssetData& asset : AssetsData) {
		if (!asset.IsAssetLoaded()) {
			const FString& NativeParentPath = asset.TagsAndValues.FindTag("NativeParentClass").AsString();
			UClass* Parent = FindObject<UClass>(NULL, *NativeParentPath);
			if (Parent &&
				(Parent->IsChildOf(UFGItemDescriptor::StaticClass())
				|| Parent->IsChildOf(UFGSchematic::StaticClass())
				|| Parent->IsChildOf(UFGResearchTree::StaticClass())
				|| Parent->IsChildOf(UFGItemCategory::StaticClass())
				|| Parent->IsChildOf(UFGRecipe::StaticClass()))
				)
			{
				const FString& assetPathString = asset.GetObjectPathString().Append("_C"); // TODOU8 Migrated asset.GetObjectPathString() from asset.ObjectPath.ToString()
				LoadObject<UClass>(NULL, *assetPathString);
			}
		}
	}
	UE_LOG(LogContentLib, Warning, TEXT("ContentLib done loading relevant FG Assets"));

	
	TArray<TSubclassOf<class UFGSchematic>> toProcess;
	TArray<UClass*> Arr;
	GetDerivedClasses(UFGSchematic::StaticClass(), Arr, true);
	for (auto i : Arr) {
		HandleSchematic(i);
	}

	static FStructProperty* NodeDataStructProperty = nullptr;
	static FClassProperty* SchematicStructProperty = nullptr;
	static UClass* ResearchTreeNodeClass = nullptr;

	ResearchTreeNodeClass = LoadClass<UFGResearchTreeNode>(
        nullptr, TEXT("/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"));
	if (ResearchTreeNodeClass) {
		TArray<UClass*> Trees;
		GetDerivedClasses(UFGResearchTree::StaticClass(), Arr, true);
		for(auto i : Trees) {
			NodeDataStructProperty = FReflectionHelper::FindPropertyChecked<FStructProperty>(
            ResearchTreeNodeClass, TEXT("mNodeDataStruct"));
			SchematicStructProperty = FReflectionHelper::FindPropertyByShortNameChecked<FClassProperty>(
                NodeDataStructProperty->Struct, TEXT("Schematic"));
			const TArray<UFGResearchTreeNode*> Nodes = UFGResearchTree::GetNodes(i);
			for (UFGResearchTreeNode* Node : Nodes) {
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
}

FFactoryGame_Recipe::FFactoryGame_Recipe(const TSubclassOf<UFGRecipe> Class, const FFactoryGame_Schematic Schematic)
{
	nRecipeClass = Class;
	nUnlockedBy.Add(Schematic.nClass);
	MJ = FFactoryGame_RecipeMJ(Class);
}

TArray<float> FFactoryGame_Recipe::GetIngredientsForProductRatio(const TSubclassOf<UFGItemDescriptor> Item) const
{
	const auto Ingredients = UFGRecipe::GetIngredients(nRecipeClass);
	const auto Products = UFGRecipe::GetProducts(nRecipeClass);
	TArray<float> Array;
	for (auto& i : Ingredients) {
		if(i.ItemClass != Item)
			continue;
		for (auto& e : Products) {
			Array.Add((float)e.Amount / i.Amount);
		}
	}
	return Array;
}

float FFactoryGame_Recipe::GetItemToTotalProductRatio(TSubclassOf<UFGItemDescriptor> Item, UContentLibSubsystem* System ) const
{
	if (!System) {
		UE_LOG(LogContentLib, Error, TEXT("------------------------FFactoryGame_Recipe nullptr Subsystem in function GetItemToTotalProductRatio ----------------------"));
		return 0.f;
	}
	//const auto CDO = nRecipeClass.GetDefaultObject();
	const auto Products = UFGRecipe::GetProducts(nRecipeClass);
	if(Products.Num() > 1) {
		if(MJ.HasAssignedMJ()) {
			for (auto& Product : Products) {
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
	if (!System) {
		UE_LOG(LogContentLib, Error, TEXT("------------------------FFactoryGame_Recipe nullptr Subsystem in function DiscoverMachines ----------------------"));
		return;
	}
	if (Products().IsValidIndex(0)) {
		for (const auto& Builder : UFGRecipe::GetProducedIn(nRecipeClass)) {
			TArray<UClass*> BuilderSubclasses;
			GetDerivedClasses(Builder, BuilderSubclasses, true);
			if (!BuilderSubclasses.Contains(Builder) && !Builder->IsNative()) {
				BuilderSubclasses.Add(Builder);
			}
			for (auto Subclass : BuilderSubclasses) {
				if (!Subclass) {
					UE_LOG(LogContentLib, Error, TEXT("When processing derived classes of %s, encountered null subclass, this is a problem with another mod"), *UKismetSystemLibrary::GetClassDisplayName(Builder.Get()));
					continue;
				}
				if (Subclass->IsChildOf(AFGBuildGun::StaticClass()) && Products()[0]->IsChildOf(UFGBuildDescriptor::StaticClass())) {
					TSubclassOf<UFGBuildDescriptor> BuildingDescriptor = *Products()[0];
					TSubclassOf<AFGBuildable> Buildable;
					Buildable = UFGBuildDescriptor::GetBuildClass(BuildingDescriptor);
					if (!System->BuildGunBuildings.Contains(*BuildingDescriptor))
						System->BuildGunBuildings.Add(Buildable, *BuildingDescriptor);
				}
			}
		}
	}
}

void FFactoryGame_Recipe::DiscoverItem(UContentLibSubsystem* System ) const
{
	if (!System) {
		UE_LOG(LogContentLib, Error, TEXT("------------------------FFactoryGame_Recipe nullptr Subsystem in function DiscoverItem ----------------------"));
		return;
	}

	for (const auto& Ingredient : Ingredients()) {
		FFactoryGame_Descriptor Item = FFactoryGame_Descriptor(Ingredient, nRecipeClass);
		if (System->Items.Contains(Ingredient)) {
			System->Items.Find(Ingredient)->IngredientInRecipe.Add(nRecipeClass);
		} else {
			System->Items.Add(Ingredient, Item);
		}
	}

	for (const auto& Product : Products()) {
		FFactoryGame_Descriptor Item = FFactoryGame_Descriptor(Product);
		if (System->Items.Contains(Product)) {
			System->Items.Find(Product)->ProductInRecipe.Add(nRecipeClass);
		} else {
			Item.ProductInRecipe.Add(nRecipeClass);
			System->Items.Add(Product, Item);
		}
	}
}

bool FFactoryGame_Recipe::IsManualOnly() const
{
	// if there is no producer at all 
	// its not manual and not manual only, however the way i wrote it,
	// it would return true if no producer which is not a workbench is found , exists as a producer 
	// so no producer provided would return true , which is undesired
	// so if there is no workbench at all, it cant be manual only and we dont have to possibly iterate all producers and can exit early 
	// - Nog
	// https://discord.com/channels/@me/677971486400577556/1166782537817931807
	if (!IsManual()) {
		return false;
	}
	
	for (auto& Producer : UFGRecipe::GetProducedIn(nRecipeClass)) {
		if (!Producer) {
			UE_LOG(LogContentLib, Warning, TEXT("When processing producers of %s, encountered null producer, this is a problem with another mod"), *UKismetSystemLibrary::GetDisplayName(nRecipeClass));
		}
		if (Producer && !Producer->IsChildOf(UFGWorkBench::StaticClass())) {
			return false;
		}
	}
	return true;
}

bool FFactoryGame_Recipe::IsManual() const
{
	for (auto& Producer : UFGRecipe::GetProducedIn(nRecipeClass)) {
		if (!Producer) {
			UE_LOG(LogContentLib, Warning, TEXT("When processing producers of %s, encountered null producer, this is a problem with another mod"), *UKismetSystemLibrary::GetDisplayName(nRecipeClass));
		}
		if (Producer && Producer->IsChildOf(UFGWorkBench::StaticClass())) {
			return true;
		}
	}
	return false;
}

bool FFactoryGame_Recipe::UnlockedFromAlternate()
{
	for (auto& Schematic : nUnlockedBy) {
		if (!Schematic) {
			UE_LOG(LogContentLib, Warning, TEXT("When processing nUnlockedBy of %s, encountered null schematic, this is a problem with another mod"), *UKismetSystemLibrary::GetDisplayName(Schematic));
		}
		if (Schematic && UFGSchematic::GetType(Schematic) == ESchematicType::EST_Alternate) {
			return true;
		}
	}
	return false;
}

bool FFactoryGame_Recipe::IsBuildGunRecipe() const
{
	for (auto& Producer : UFGRecipe::GetProducedIn(nRecipeClass)) {
		if (!Producer) {
			UE_LOG(LogContentLib, Warning, TEXT("When processing producers of %s, encountered null producer, this is a problem with another mod"), *UKismetSystemLibrary::GetDisplayName(nRecipeClass));
		}
		if (Producer && Producer->IsChildOf(AFGBuildGun::StaticClass())) {
			return true;
		}
	}

	return false;
}

TArray<TSubclassOf<UFGItemDescriptor>> FFactoryGame_Recipe::Products() const
{
	TArray<TSubclassOf<UFGItemDescriptor>> out;
	for (const auto& ProductStruct : UFGRecipe::GetProducts(nRecipeClass)) {
		if (ProductStruct.ItemClass) {
			out.Add(ProductStruct.ItemClass);
		}
	}
	return out;
}

TArray<TSubclassOf<UFGItemDescriptor>> FFactoryGame_Recipe::Ingredients() const
{
	TArray<TSubclassOf<UFGItemDescriptor>> out;
	for (const auto& IngredientStruct : UFGRecipe::GetIngredients(nRecipeClass)) {
		if (IngredientStruct.ItemClass) {
			out.Add(IngredientStruct.ItemClass);
		}
	}
	return out;
}

TArray<TSubclassOf<UFGItemCategory>> FFactoryGame_Recipe::ProductCats() const
{
	TArray<TSubclassOf<class UFGItemCategory>> Out;
	for (const auto& Product : UFGRecipe::GetProducts(nRecipeClass)) {
		if (TSubclassOf<UFGItemCategory> ItemCat = *UFGItemDescriptor::GetCategory(Product.ItemClass)) {
			Out.AddUnique(ItemCat);
		}
	}
	return Out;
}

TArray<TSubclassOf<UFGItemCategory>> FFactoryGame_Recipe::IngredientCats() const
{
	TArray<TSubclassOf<class UFGItemCategory>> Out;
	for (const auto& Ingredient : UFGRecipe::GetIngredients(nRecipeClass)) {
		if (TSubclassOf<UFGItemCategory> ItemCat = *UFGItemDescriptor::GetCategory(Ingredient.ItemClass)) {
			Out.AddUnique(ItemCat);
		}
	}
	return Out;
}

FFactoryGame_Schematic::FFactoryGame_Schematic() {}

FFactoryGame_Schematic::FFactoryGame_Schematic(TSubclassOf<UFGSchematic> inClass, UContentLibSubsystem* System)
{
	nClass = inClass;
	DiscoverUnlocks(System);
}

void FFactoryGame_Schematic::DiscoverUnlocks(UContentLibSubsystem* System)
{
	GatherDependencies();

	for (UFGUnlock* Unlock : nClass.GetDefaultObject()->GetUnlocks(nClass)) {
		// Recipe unlocks make struct for it and save Buildings found
		if (UFGUnlockRecipe* UnlockRecipe = Cast<UFGUnlockRecipe>(Unlock)) {
			for (TSubclassOf<UFGRecipe> UnlockedRecipe : UnlockRecipe->GetRecipesToUnlock()) {
				if (!UnlockedRecipe) {
					continue;
				}
				if (!System->Recipes.Contains(UnlockedRecipe)) {
					FFactoryGame_Recipe RecipeStruct = FFactoryGame_Recipe(UnlockedRecipe, *this);
					RecipeStruct.DiscoverItem(System);
					RecipeStruct.DiscoverMachines(System);
					System->Recipes.Add(UnlockedRecipe, RecipeStruct);
				} else {
					System->Recipes.Find(UnlockedRecipe)->nUnlockedBy.Add(nClass);
				}
			}
		} else if (UFGUnlockSchematic* UnlockSchematic = Cast<UFGUnlockSchematic>(Unlock)) { // schematics unlocks cause recursion
			for (TSubclassOf<UFGSchematic> UnlockedSchematic : UnlockSchematic->GetSchematicsToUnlock()) {
				if (!System->Schematics.Contains(UnlockedSchematic)) {
					System->HandleSchematic(UnlockedSchematic);
				}
			}
		}
	}
}

void FFactoryGame_Schematic::GatherDependencies()
{
	TArray<UFGAvailabilityDependency*> Out_SchematicDependencies;
	nClass.GetDefaultObject()->GetSchematicDependencies(nClass, Out_SchematicDependencies);
	for (auto Dependency : Out_SchematicDependencies) {
		if (!Dependency) {
			continue;
		}
		if (const UFGSchematicPurchasedDependency* Dep = Cast<UFGSchematicPurchasedDependency>(Dependency)) {
			TArray<TSubclassOf<UFGSchematic>> Out_Schematics;
			Dep->GetSchematics(Out_Schematics);
			for (TSubclassOf<UFGSchematic> Schematic : Out_Schematics) {
				if (!Schematic) {
					continue;
				}
				if (!nDependsOn.Contains(Schematic)) {
					nDependsOn.Add(Schematic);
				}
			}
		}
	}
}
