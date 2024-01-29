


#include "CLRecipeBPFLib.h"
#include "ContentLib.h"

#include "FGSchematic.h"
#include "Unlocks/FGUnlockRecipe.h"

#include "BPFContentLib.h"
#include "FGSchematicManager.h"
#include "FGUnlockSubsystem.h"
#include "FGWorkBench.h"
#include "ContentLibSubsystem.h"
#include "Buildables/FGBuildableFactory.h"
#include "Registry/ModContentRegistry.h"

void UCLRecipeBPFLib::InitRecipeFromStruct(UContentLibSubsystem* Subsystem ,FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe,bool ClearIngredients,bool ClearProducts,bool ClearBuilders)
{
	if (!Recipe) {
		return;
	}
	UFGRecipe* CDO = Recipe.GetDefaultObject();

	// If a Name is specified, it will also turn on override
	if (RecipeStruct.Name != "") {
		CDO->mDisplayName = FText::FromString(RecipeStruct.Name);
		CDO->mDisplayNameOverride = true;
	}
	// You can also turn override on/off manually. It defaults to -1 which won't trip either of these
	if (RecipeStruct.OverrideName == 1) {
		CDO->mDisplayNameOverride = true;
	} else if (RecipeStruct.OverrideName == 0) {
		CDO->mDisplayNameOverride = false;
	}

	if (RecipeStruct.Category != "")
	{
		const TSubclassOf<UFGItemCategory> Out = UBPFContentLib::SetCategoryWithLoad(RecipeStruct.Category, Subsystem, false);
		if (Out) {
			CDO->mOverriddenCategory = Out;
		}
		if (!CDO->mOverriddenCategory) {
			UE_LOG(LogContentLib, Error, TEXT("Recipe Category Override probably failed; a category was specified in the struct but was still nullptr after apply"));
		}
	}
	UBPFContentLib::AddToItemAmountArray(Recipe.GetDefaultObject()->mIngredients,RecipeStruct.Ingredients,Subsystem->mItems,ClearIngredients);
	UBPFContentLib::AddToItemAmountArray(Recipe.GetDefaultObject()->mProduct,RecipeStruct.Products,Subsystem->mItems,ClearProducts);
	AddBuilders(Recipe,RecipeStruct,Subsystem->mBuilders,Subsystem->mCraftingComps, ClearBuilders);
	AddToSchematicUnlock(Recipe,RecipeStruct,Subsystem);
	if(RecipeStruct.ManufacturingDuration != -1)
		CDO->mManufactoringDuration = RecipeStruct.ManufacturingDuration;
	if (RecipeStruct.ManualManufacturingMultiplier != -1)
		CDO->mManualManufacturingMultiplier = RecipeStruct.ManualManufacturingMultiplier;
	if (RecipeStruct.VariablePowerConsumptionConstant != -1)
		CDO->mVariablePowerConsumptionConstant = RecipeStruct.VariablePowerConsumptionConstant;
	if (RecipeStruct.VariablePowerConsumptionFactor != -1)
		CDO->mVariablePowerConsumptionFactor = RecipeStruct.VariablePowerConsumptionFactor;
	if (RecipeStruct.ManufacturingMenuPriority != -1)
		CDO->mManufacturingMenuPriority = RecipeStruct.ManufacturingMenuPriority;

}



void UCLRecipeBPFLib::AddBuilders(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, const bool ClearFirst)
{
	if (!Recipe)
		return;
	if (ClearFirst)
		Recipe.GetDefaultObject()->mProducedIn.Empty();

	for(auto& i : RecipeStruct.BuildIn) {

		if(i.Contains("/")) {
			UClass* Loaded = LoadObject<UClass>(nullptr, *RecipeStruct.Category);
			if (Loaded && Loaded->IsChildOf(UFGSchematic::StaticClass()) && !Recipe.GetDefaultObject()->mProducedIn.Contains((Loaded)))
					Recipe.GetDefaultObject()->mProducedIn.Add(Loaded);
			else if(!Recipe.GetDefaultObject()->mProducedIn.Contains(Loaded)) {
				UE_LOG(LogContentLib,Error,TEXT("Finding Builder by Path %s failed"), *i);
			}	
		}
		else
		{
			bool Found = false;
			for(auto e : Builders) {
				if (UBPFContentLib::StringCompareItem(e->GetName(),i,"Build","_C")) {
					TSoftClassPtr< UObject > Insert = TSoftClassPtr< UObject > (e);
					if(!Recipe.GetDefaultObject()->mProducedIn.Contains((Insert)))
						Recipe.GetDefaultObject()->mProducedIn.Add(Insert);
					Found = true;
					break;
				}
			}
			for(auto e : CraftingComps) {
				TSubclassOf<class UFGWorkBench> Desc = e;
				if(Desc.GetDefaultObject()->GetName().Equals(i, ESearchCase::IgnoreCase)|| i.Equals("manual", ESearchCase::IgnoreCase)) {
					TSoftClassPtr< UObject > Insert = TSoftClassPtr< UObject > (e);
					if(!Recipe.GetDefaultObject()->mProducedIn.Contains((Insert)))
						Recipe.GetDefaultObject()->mProducedIn.Add(Insert);
					Found = true;
					break;
				}
			}
			if (i.Contains("BP_BuildGun") || i.Contains("BuildGun") || i.Contains("FGBuildGun")) {
				UClass* Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/Equipment/BuildGun/BP_BuildGun.BP_BuildGun_C"));
				if (!Recipe.GetDefaultObject()->mProducedIn.Contains((Class)))
					Recipe.GetDefaultObject()->mProducedIn.Add(Class);
				Found = true;
			}

			if (!Found) {
				UE_LOG(LogContentLib, Error, TEXT("CL Recipes: Failed to find Builder %s for Recipe %s"), *i, *Recipe->GetName());
			}

		}
	}
}

void UCLRecipeBPFLib::AddToSchematicUnlock(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct, UContentLibSubsystem* Subsystem)
{
	if (!Recipe) {
		return;
	}
	for (const FString& SchematicToFind : RecipeStruct.UnlockedBy) {
		UClass * SchematicClass = UBPFContentLib::FindClassWithLog(SchematicToFind,UFGSchematic::StaticClass(),Subsystem);
		if (SchematicClass) {
			UBPFContentLib::AddRecipeToUnlock(SchematicClass, Subsystem, Recipe);
		} else {
			UE_LOG(LogContentLib, Error, TEXT("CL Recipes: Failed to find Schematic for UnlockedBy with name %s for Recipe %s"), *SchematicToFind, *Recipe->GetName());
		}
	}
};


FContentLib_Recipe UCLRecipeBPFLib::GenerateCLRecipeFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_Recipe();
	}
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> ParsedJson;
	Serializer.Deserialize(Reader, ParsedJson);
	if(!ParsedJson.IsValid()) {
		UE_LOG(LogContentLib, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_Recipe();
	}

	if (ParsedJson->HasField("Category")) {
		UE_LOG(LogContentLib, Error, TEXT("You are using the outdated Recipe field 'Category'! It has been renamed to 'OverrideCategory'. This will be a real error soon, but for now it still works."), *String);
		ParsedJson->SetStringField("OverrideCategory", ParsedJson->TryGetField("Category")->AsString());
	}

	FContentLib_Recipe Recipe;

	UBPFContentLib::SetStringFieldWithLog(Recipe.Name, "Name", ParsedJson);
	UBPFContentLib::SetIntegerFieldWithLog(Recipe.OverrideName, "OverrideName", ParsedJson);
	UBPFContentLib::SetStringFieldWithLog(Recipe.Category, "OverrideCategory", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Recipe.ManufacturingDuration, "ManufacturingDuration", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Recipe.ManualManufacturingMultiplier, "ManualManufacturingMultiplier", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Recipe.VariablePowerConsumptionFactor, "VariablePowerConsumptionFactor", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Recipe.VariablePowerConsumptionConstant, "VariablePowerConsumptionConstant", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Recipe.ManufacturingMenuPriority, "ManufacturingMenuPriority", ParsedJson);
	if (!UBPFContentLib::SetStringIntMapFieldWithLog(Recipe.Ingredients, "Ingredients", ParsedJson))
		Recipe.ClearIngredients = false;
	if (!UBPFContentLib::SetStringIntMapFieldWithLog(Recipe.Products, "Products", ParsedJson))
		Recipe.ClearProducts = false;

	if (!UBPFContentLib::SetStringArrayFieldWithLog(Recipe.BuildIn, "ProducedIn", ParsedJson))
		Recipe.ClearBuilders = false;
	
	UBPFContentLib::SetStringArrayFieldWithLog(Recipe.UnlockedBy, "UnlockedBy", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Recipe.ClearIngredients, "ClearIngredients", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Recipe.ClearProducts, "ClearProducts", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Recipe.ClearBuilders, "ClearBuilders", ParsedJson);
	
	return Recipe;
}

FString UCLRecipeBPFLib::SerializeRecipe(const TSubclassOf<UFGRecipe> Recipe)
{
	if (!Recipe)
		return "";
	const auto CDO = Cast<UFGRecipe>(Recipe->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto Override = MakeShared<FJsonValueBoolean>(CDO->mDisplayNameOverride);
	const auto Cat = MakeShared<FJsonValueString>(CDO->mOverriddenCategory->GetPathName());
	const auto ManufacturingDuration = MakeShared<FJsonValueNumber>(CDO->mManufactoringDuration);
	const auto mVariablePowerConsumptionFactor = MakeShared<FJsonValueNumber>(CDO->mVariablePowerConsumptionFactor);
	const auto mVariablePowerConsumptionConstant = MakeShared<FJsonValueNumber>(CDO->mVariablePowerConsumptionConstant);

	TArray< TSharedPtr<FJsonValue>> Ingredients;
	TArray< TSharedPtr<FJsonValue>> Products;
	TArray< TSharedPtr<FJsonValue>> ProducedIn; 
	for(auto& i : CDO->mIngredients) {
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.ItemClass->GetName()));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Amount));
		Ingredients.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto& i : CDO->mProduct) {
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.ItemClass->GetName()));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Amount));
		Products.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto& i : CDO->mProducedIn) {
		auto IngObj = MakeShared<FJsonObject>();
		ProducedIn.Add(MakeShared<FJsonValueString>(i->GetPathName()));
	}
	const auto Ing = MakeShared<FJsonValueArray>(Ingredients);
	const auto Prod = MakeShared<FJsonValueArray>(Products);
	const auto Producer = MakeShared<FJsonValueArray>(ProducedIn);

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("OverrideName", Override);
	Obj->Values.Add("Category", Cat);
	Obj->Values.Add("Ingredients",Ing);
	Obj->Values.Add("Products",Prod);
	Obj->Values.Add("ProducedIn",Producer);
	Obj->Values.Add("ManufacturingDuration", ManufacturingDuration);
	Obj->Values.Add("VariablePowerConsumptionFactor", mVariablePowerConsumptionFactor);
	Obj->Values.Add("VariablePowerConsumptionConstant", mVariablePowerConsumptionConstant);
	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UCLRecipeBPFLib::SerializeCLRecipe(FContentLib_Recipe Recipe)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(Recipe.Name);
	const auto Override = MakeShared<FJsonValueBoolean>(static_cast<bool>(Recipe.OverrideName));
	const auto Cat = MakeShared<FJsonValueString>(Recipe.Category);
	const auto ManufacturingDuration = MakeShared<FJsonValueNumber>(Recipe.ManufacturingDuration);
	const auto mVariablePowerConsumptionFactor = MakeShared<FJsonValueNumber>(Recipe.VariablePowerConsumptionFactor);
	const auto mVariablePowerConsumptionConstant = MakeShared<FJsonValueNumber>(Recipe.VariablePowerConsumptionConstant);

	
	TArray< TSharedPtr<FJsonValue>> Ingredients;
	TArray< TSharedPtr<FJsonValue>> Products;
	TArray< TSharedPtr<FJsonValue>> ProducedIn; 
	for(auto& i : Recipe.Ingredients) {
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.Key));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Value));
		Ingredients.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto& i : Recipe.Products) {
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.Key));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Value));
		Products.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	for(auto& i : Recipe.BuildIn) {
		auto IngObj = MakeShared<FJsonObject>();
		ProducedIn.Add(MakeShared<FJsonValueString>(i));
	}
	const auto Ing = MakeShared<FJsonValueArray>(Ingredients);
	const auto Prod = MakeShared<FJsonValueArray>(Products);
	const auto Producer = MakeShared<FJsonValueArray>(ProducedIn);
	const auto ClearIngredients = MakeShared<FJsonValueBoolean>(Recipe.ClearIngredients);
	const auto ClearProducts = MakeShared<FJsonValueBoolean>(Recipe.ClearProducts);
	const auto ClearBuilders = MakeShared<FJsonValueBoolean>(Recipe.ClearBuilders);



	
	Obj->Values.Add("Name",Name);
	Obj->Values.Add("OverrideName", Override);
	Obj->Values.Add("Category", Cat);
	Obj->Values.Add("Ingredients",Ing);
	Obj->Values.Add("Products",Prod);
	Obj->Values.Add("ProducedIn",Producer);
	Obj->Values.Add("ManufacturingDuration", ManufacturingDuration);
	Obj->Values.Add("VariablePowerConsumptionFactor", mVariablePowerConsumptionFactor);
	Obj->Values.Add("VariablePowerConsumptionConstant", mVariablePowerConsumptionConstant);
	Obj->Values.Add("ClearIngredients", ClearIngredients);
	Obj->Values.Add("ClearProducts", ClearProducts);
	Obj->Values.Add("ClearBuilders", ClearBuilders);

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
};
