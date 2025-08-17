


#include "CLSchematicBPFLib.h"
#include "ContentLib.h"

#include "FGSchematicCategory.h"
#include "BPFContentLib.h"
#include "FGResearchTree.h"
#include "AvailabilityDependencies/FGSchematicPurchasedDependency.h"
#include "Reflection/ReflectionHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Unlocks/FGUnlockRecipe.h"



FContentLib_ResearchNode UCLSchematicBPFLib::GenerateResearchStructFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_ResearchNode();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid()) {
		return FContentLib_ResearchNode();
	}

	FContentLib_ResearchNode NodeStruct;

	UBPFContentLib::SetStringFieldWithLog(NodeStruct.ResearchTree, "ResearchTree", Result);

	for (const auto& i : Result->TryGetField("Parents")->AsArray()) {
		if (i->Type == EJson::Object) {
			if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y")) {
				FContentLib_Vector2D Vector2D;

				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", Result);
				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", Result);
				NodeStruct.Parents.Add(Vector2D);
			}
		}
	}

	for (const auto& i : Result->TryGetField("Children")->AsArray()) {
		if (i->Type == EJson::Object) {
			if (i->AsObject()->HasField("Child") && i->AsObject()->HasField("Roads")) {
				FContentLib_ResearchNodeRoads Node;
				FContentLib_Vector2D Key;
				if (i->AsObject()->TryGetField("Child")->Type == EJson::Object) {
					TSharedPtr<FJsonObject> Child = i->AsObject()->TryGetField("Child")->AsObject();
					if (Child->HasField("X") && Child->TryGetField("X")->Type == EJson::Number &&
						Child->HasField("Y") && Child->TryGetField("Y")->Type == EJson::Number)
					{
						UBPFContentLib::SetIntegerFieldWithLog(Key.X, "X", Child);
						UBPFContentLib::SetIntegerFieldWithLog(Key.Y, "Y", Child);
					}
				}
				Node.ChildNode = Key;

				TArray<FContentLib_Vector2D> Values;
				for (TSharedPtr<FJsonValue> e : i->AsObject()->TryGetField("Roads")->AsArray()) {
					if (e->Type == EJson::Object) {
						if (e->AsObject()->HasField("X") && e->AsObject()->HasField("Y")) {
							FContentLib_Vector2D Vector2D;
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", Result);
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", Result);
							Node.Roads.Add(Vector2D);
						}
					}
				}
				NodeStruct.Children.Add(Node);
			}
		}
	}
	return NodeStruct;
}

FContentLib_ResearchNodeRoads UCLSchematicBPFLib::GenerateResearchNodeRoadsFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_ResearchNodeRoads();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid()) {
		return FContentLib_ResearchNodeRoads();
	}

	FContentLib_ResearchNodeRoads Roads;
	for (const auto& i : Result->TryGetField("Roads")->AsArray()) {
		if (i->Type == EJson::Object) {
			if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y")) {
				FContentLib_Vector2D Vector2D;

				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
				Roads.Roads.Add(Vector2D);
			}
		}
	}
	return Roads;
}


FContentLib_Vector2D UCLSchematicBPFLib::GenerateVector2DFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_Vector2D();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid()) {
		return FContentLib_Vector2D();
	}

	FContentLib_Vector2D Vector2D;
	UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", Result);
	return Vector2D;
}

FContentLib_Schematic UCLSchematicBPFLib::GenerateCLSchematicFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_Schematic();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> ParsedJson;
	Serializer.Deserialize(Reader, ParsedJson);
	if (!ParsedJson.IsValid()) {
		return FContentLib_Schematic();
	}

	if (auto LegacyCategoryField = ParsedJson->TryGetField("Cat")) {
		UE_LOG(LogContentLib, Error, TEXT("You are using the outdated Schematic field 'Cat'! It has been renamed to 'Category'. This will be a real error soon, but for now it still works."), *String);
		ParsedJson->SetStringField("Category", LegacyCategoryField->AsString());
	}

	if (auto LegacySubCategoriesField = ParsedJson->TryGetField("SubCat")) {
		UE_LOG(LogContentLib, Error, TEXT("You are using the outdated Schematic field 'SubCat'! It has been renamed to 'SubCategories'. This will be a real error soon, but for now it still works."), *String);
		ParsedJson->SetArrayField("SubCategories", LegacySubCategoriesField->AsArray());
	}

	if (auto LegacyInventorySlotsField = ParsedJson->TryGetField("SlotsToGive")) {
		UE_LOG(LogContentLib, Error, TEXT("You are using the outdated Schematic field 'SlotsToGive'! It has been renamed to 'InventorySlotsToGive'. This will be a real error soon, but for now it still works."), *String);
		ParsedJson->SetNumberField("InventorySlotsToGive", LegacyInventorySlotsField->AsNumber());
	}

	if (auto LegacyClearSubCategoriesField = ParsedJson->TryGetField("ClearCats")) {
		UE_LOG(LogContentLib, Error, TEXT("You are using the outdated Schematic field 'ClearCats'! It has been renamed to 'ClearSubCategories'. This will be a real error soon, but for now it still works."), *String);
		ParsedJson->SetBoolField("ClearSubCategories", LegacyClearSubCategoriesField->AsBool());
	}

	FContentLib_Schematic Schematic;

	UBPFContentLib::SetStringFieldWithLog(Schematic.Type, "Type", ParsedJson);
	UBPFContentLib::SetStringFieldWithLog(Schematic.Name, "Name", ParsedJson);
	UBPFContentLib::SetStringFieldWithLog(Schematic.VisualKit, "VisualKit", ParsedJson);
	UBPFContentLib::SetStringFieldWithLog(Schematic.Description, "Description", ParsedJson);
	UBPFContentLib::SetStringFieldWithLog(Schematic.Category, "Category", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Schematic.MenuPriority, "MenuPriority", ParsedJson);
	UBPFContentLib::SetFloatFieldWithLog(Schematic.Time, "Time", ParsedJson);
	UBPFContentLib::SetIntegerFieldWithLog(Schematic.Tier, "Tier", ParsedJson);
	UBPFContentLib::SetIntegerFieldWithLog(Schematic.InventorySlotsToGive, "InventorySlotsToGive", ParsedJson);
	UBPFContentLib::SetIntegerFieldWithLog(Schematic.ArmSlotsToGive, "ArmSlotsToGive", ParsedJson);

	// specifying these fields causes different default values for the Clear flags
	if (!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.SubCategories, "SubCategories", ParsedJson))
		Schematic.ClearSubCategories = false;
	if (!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.Recipes, "Recipes", ParsedJson))
		Schematic.ClearRecipes = false;
	if (!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.Schematics, "Schematics", ParsedJson))
		Schematic.ClearSchematics = false;
	if (!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.DependsOn, "DependsOn", ParsedJson))
		Schematic.ClearDeps = false;
	if (!UBPFContentLib::SetStringIntMapFieldWithLog(Schematic.Cost, "Cost", ParsedJson))
		Schematic.ClearCost = false;
	if (!UBPFContentLib::SetStringIntMapFieldWithLog(Schematic.ItemsToGive, "ItemsToGive", ParsedJson))
		Schematic.ClearItemsToGive = false;

	// set the clear flags
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearSubCategories, "ClearSubCategories", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearCost, "ClearCost", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearRecipes, "ClearRecipes", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearSchematics, "ClearSchematics", ParsedJson);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearDeps, "ClearDeps", ParsedJson); // TODO is this even used?
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearItemsToGive, "ClearItemsToGive", ParsedJson);

	if (ParsedJson->HasField("ResearchTree")) {
		const auto TResult = ParsedJson->TryGetField("ResearchTree");
		UBPFContentLib::SetStringFieldWithLog(Schematic.ResearchTree.ResearchTree, "ResearchTree", TResult->AsObject());

		if (TResult->Type == EJson::Object) {
			const auto TTResult = TResult->AsObject()->TryGetField("Coords");

			if (TTResult->Type == EJson::Object) {
				if (TTResult->AsObject()->HasField("X") && TTResult->AsObject()->HasField("Y")) {
					FContentLib_Vector2D Vector2D;

					UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", TTResult->AsObject());
					UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", TTResult->AsObject());
					Schematic.ResearchTree.Coordinates = Vector2D;
				}
			}
			if (TResult->AsObject()->HasField("Parents") && TResult->AsObject()->TryGetField("Parents")->Type == EJson::Array) {
				for (const auto& i : TResult->AsObject()->TryGetField("Parents")->AsArray()) {
					if (i->Type == EJson::Object) {
						if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y")) {
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
							Schematic.ResearchTree.Parents.Add(Vector2D);
						}
					}
				}
			}
			if (TResult->AsObject()->HasField("UnHiddenBy") && TResult->AsObject()->TryGetField("UnHiddenBy")->Type == EJson::Array) {
				for (const auto& i : TResult->AsObject()->TryGetField("UnHiddenBy")->AsArray()) {
					if (i->Type == EJson::Object) {
						if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y")) {
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
							Schematic.ResearchTree.UnHiddenBy.Add(Vector2D);
						}
					}
				}
			}

			if (TResult->AsObject()->HasField("NodesToUnHide") && TResult->AsObject()->TryGetField("NodesToUnHide")->Type == EJson::Array) {
				for (const auto& i : TResult->AsObject()->TryGetField("NodesToUnHide")->AsArray()) {
					if (i->Type == EJson::Object) {
						if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y")) {
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
							Schematic.ResearchTree.NodesToUnHide.Add(Vector2D);
						}
					}
				}
			}
			if (TResult->AsObject()->HasField("Children") && TResult->AsObject()->TryGetField("Children")->Type == EJson::Array) {
				for (const auto& i : TResult->AsObject()->TryGetField("Children")->AsArray()) {
					if (i->Type == EJson::Object) {
						if (i->AsObject()->HasField("ChildNode") && i->AsObject()->HasField("Roads")) {
							FContentLib_Vector2D Key;
							if (i->AsObject()->TryGetField("ChildNode")->Type == EJson::Object) {
								UBPFContentLib::SetIntegerFieldWithLog(Key.X, "X", i->AsObject()->TryGetField("ChildNode")->AsObject());
								UBPFContentLib::SetIntegerFieldWithLog(Key.Y, "Y", i->AsObject()->TryGetField("ChildNode")->AsObject());
							}

							TArray<FContentLib_Vector2D> Values;
							for (const auto& e : i->AsObject()->TryGetField("Roads")->AsArray()) {
								if (e->Type == EJson::Object) {
									FContentLib_Vector2D Vector2D;
									UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", e->AsObject());
									UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", e->AsObject());
									Values.Add(Vector2D);
								}
							}
							FContentLib_ResearchNodeRoads Value;
							Value.Roads = Values;
							Value.ChildNode = Key;
							Schematic.ResearchTree.Children.Add(Value);
						}
					}
				}
			}
		}
	}
	return Schematic;
}

static void SetSchematicTypeFromString(ESchematicType& Type, const FString& Name)
{
	using enum ESchematicType;

	// TODO: Since we're using IgnoreCase maybe we could use FName instead.
	//       But I don't know if this is worth the performance gains. [Rex]
	static const TArray<TPair<FString, ESchematicType>> LookupTable = {
		{ "Custom",        EST_Custom        },
		{ "Cheat",         EST_Cheat         },
		{ "Tutorial",      EST_Tutorial      },
		{ "Milestone",     EST_Milestone     },
		{ "Alternate",     EST_Alternate     },
		{ "Story",         EST_Story         },
		{ "MAM",           EST_MAM           },
		{ "ResourceSink",  EST_ResourceSink  },
		{ "HardDrive",     EST_HardDrive     },
		{ "Prototype",     EST_Prototype     },
		{ "Customization", EST_Customization },
	};

	const auto lambda = [&Name](const TPair<FString, ESchematicType>& e) {
		return Name.Equals(e.Key, ESearchCase::IgnoreCase);
	};
	if (const TPair<FString, ESchematicType>* Result = LookupTable.FindByPredicate(lambda)) {
		Type = Result->Value;
	} else {
		UE_LOG(LogContentLib, Error, TEXT("Unknown Schematic Type %s"), *Name);
	}
}

void UCLSchematicBPFLib::InitSchematicFromStruct(FContentLib_Schematic Schematic, TSubclassOf<UFGSchematic> SchematicClass, UContentLibSubsystem* SubSystem)
{
	UFGSchematic* CDO = SchematicClass.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	if (Schematic.Type != "") {
		SetSchematicTypeFromString(CDO->mType, Schematic.Type);
	}

	if (Schematic.Name != "") {
		CDO->mDisplayName = FText::FromString(Schematic.Name);
	}

	if (Schematic.Description != "") {
		CDO->mDescription = FText::FromString(Schematic.Description);
	}

	if (Schematic.Category != "") {
		if (const TSubclassOf<UFGSchematicCategory> Out = UBPFContentLib::SetCategoryWithLoad(Schematic.Category, SubSystem, true)) {
			CDO->mSchematicCategory = Out;
		}
		if (!CDO->mSchematicCategory) {
			UE_LOG(LogContentLib, Error, TEXT("Schematic Category probably failed; a category was specified in the struct but was still nullptr after apply"));
		}
	}
	if (!CDO->mSchematicCategory && Schematic.Type.Equals("ResourceSink", ESearchCase::IgnoreCase)) {
		UE_LOG(LogContentLib, Warning, TEXT("Schematic Category is blank, this means it won't show up unless searched for in the AWESOME Shop"));
	}

	if (Schematic.ClearSubCategories) {
		CDO->mSubCategories.Empty();
	}
	if (Schematic.SubCategories.Num() > 0) {
		for (const auto& categoryString : Schematic.SubCategories) {
			if (TSubclassOf<UFGSchematicCategory> Out = UBPFContentLib::SetCategoryWithLoad(categoryString, SubSystem, true)) {
				CDO->mSubCategories.Add(Out);
			} else {
				UE_LOG(LogContentLib, Error, TEXT("Failed to resolve a sub-category %s"), *categoryString);
			}
		}
		if (CDO->mSubCategories.Num() == 0) {
			UE_LOG(LogContentLib, Error, TEXT("Schematic Sub Category probably failed; some were specified in the struct but array was still empty after apply"));
		}
	}
	if (CDO->mSubCategories.Num() == 0 && Schematic.Type.Equals("ResourceSink", ESearchCase::IgnoreCase)) {
		UE_LOG(LogContentLib, Error, TEXT("Schematic Sub Category array is empty or not an array, this means it won't show up unless searched for in the AWESOME Shop"));
	}

	if (Schematic.MenuPriority != -1) {
		CDO->mMenuPriority = Schematic.MenuPriority;
	}

	if (Schematic.Tier != -1) {
		CDO->mTechTier = Schematic.Tier;
	}

	UBPFContentLib::AddToItemAmountArray(CDO->mCost, Schematic.Cost, SubSystem->mItems, Schematic.ClearCost);

	if (Schematic.Time != -1) {
		CDO->mTimeToComplete = Schematic.Time;
	}

	if (Schematic.ClearRecipes) {
		auto elementIsRecipeUnlock = [](UFGUnlock* unlock) { return Cast<UFGUnlockRecipe>(unlock); };
		CDO->mUnlocks.RemoveAll(elementIsRecipeUnlock);
	}
	for (const FString& i : Schematic.Recipes) {
		if (UClass* RecipesClass = UBPFContentLib::FindClassWithLog(i, UFGRecipe::StaticClass(), SubSystem)) {
			UBPFContentLib::AddRecipeToUnlock(SchematicClass, SubSystem, RecipesClass);
		}
	}
	
	for (const auto& entry : Schematic.InfoCards) {
		UBPFContentLib::AddInfoOnlyToUnlock(SchematicClass, SubSystem, entry);
	}

	if (Schematic.ClearSchematics) {
		auto elementIsSchematicUnlock = [](UFGUnlock* unlock) { return Cast<UFGUnlockSchematic>(unlock); };
		CDO->mUnlocks.RemoveAll(elementIsSchematicUnlock);
	}
	for (const FString& i : Schematic.Schematics) {
		if (UClass* Class = UBPFContentLib::FindClassWithLog(i, UFGSchematic::StaticClass(), SubSystem)) {
			UBPFContentLib::AddSchematicToUnlock(SchematicClass, SubSystem, Class);
		}
	}

	if (Schematic.ItemsToGive.Num() || Schematic.ClearItemsToGive) {
		UBPFContentLib::AddGiveItemsToUnlock(SchematicClass, SubSystem, Schematic.ItemsToGive, Schematic.ClearItemsToGive);
	}

	if (Schematic.InventorySlotsToGive > -1) {
		UBPFContentLib::AddInventorySlotsToUnlock(SchematicClass, SubSystem, Schematic.InventorySlotsToGive);
	}

	if (Schematic.ArmSlotsToGive > -1) {
		UBPFContentLib::AddArmSlotsToUnlock(SchematicClass, SubSystem, Schematic.ArmSlotsToGive);
	}

	if (Schematic.ClearDeps) {
		CDO->mSchematicDependencies.Empty();
	}
	for (const FString& i : Schematic.DependsOn) {
		UClass* SchematicDep = UBPFContentLib::FindClassWithLog(i, UFGSchematic::StaticClass(), SubSystem);
		if (SchematicDep && SchematicDep->IsChildOf(UFGSchematic::StaticClass())) {
			UBPFContentLib::AddSchematicToPurchaseDep(SchematicClass, SubSystem, SchematicDep);
		}
	}

	if (Schematic.VisualKit != "") {
		if (Schematic.VisualKit.Contains("{") && Schematic.VisualKit.Contains("}")) {
			const FContentLib_VisualKit Kit = UCLItemBPFLib::GenerateKitFromString(Schematic.VisualKit);
			ApplyVisualKitToSchematic(SubSystem, Kit, SchematicClass);
		} else {
			if (SubSystem->ImportedVisualKits.Contains(Schematic.VisualKit)) {
				ApplyVisualKitToSchematic(SubSystem, *SubSystem->ImportedVisualKits.Find(Schematic.VisualKit), SchematicClass);
			} else if (SubSystem->VisualKits.Contains(Schematic.VisualKit)) {
				ApplyVisualKitToSchematic(SubSystem, *SubSystem->VisualKits.Find(Schematic.VisualKit), SchematicClass);
			}
		}
	}

	if (Schematic.ResearchTree.ResearchTree != "") {
		UClass* SchematicDep = UBPFContentLib::FindClassWithLog(Schematic.ResearchTree.ResearchTree, UFGResearchTree::StaticClass(), SubSystem);
		if (SchematicDep && SchematicDep->IsChildOf(UFGResearchTree::StaticClass())) {
			static FStructProperty* NodeDataStructProperty = nullptr;
			static FClassProperty* SchematicStructProperty = nullptr;
			static UClass* ResearchTreeNodeClass = nullptr;

			ResearchTreeNodeClass = LoadClass<UFGResearchTreeNode>(
				nullptr, TEXT("/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"));
			if (ResearchTreeNodeClass) {
				NodeDataStructProperty = FReflectionHelper::FindPropertyChecked<FStructProperty>(
					ResearchTreeNodeClass, TEXT("mNodeDataStruct"));
				SchematicStructProperty = FReflectionHelper::FindPropertyByShortNameChecked<FClassProperty>(
					NodeDataStructProperty->Struct, TEXT("Schematic"));
				TArray<UFGResearchTreeNode*>& Nodes = Cast<UFGResearchTree>(SchematicDep->GetDefaultObject())->mNodes;
				bool Found = false;
				for (UFGResearchTreeNode* Node : Nodes) {
					if (!Node)
						continue;
					TSubclassOf<UFGSchematic> mSchematic = Cast<UClass>(
						SchematicStructProperty->GetPropertyValue_InContainer(
							NodeDataStructProperty->ContainerPtrToValuePtr<void>(Node)));
					if (!mSchematic || mSchematic != SchematicClass)
						continue;
					Found = true;
					SubSystem->HandleResearchTreeNodeChange(Node, Schematic.ResearchTree, SchematicClass, SchematicDep);
				}
				if (!Found) {
					UClass* Class = FindObject<UClass>(FindPackage(nullptr, TEXT("/Game/")), TEXT("BPD_ResearchTreeNode"), false);
					if (!Class) {
						//Blueprint'/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode'
						Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"));
						if (!Class) {
							UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldnt find /Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"))
						}
					}
					UFGResearchTreeNode* Node = NewObject<UFGResearchTreeNode>(SchematicDep->GetDefaultObject(), Class);
					Nodes.Add(Node);
					UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Research Tree Node. Added Schematic to %s in ResearchTree %s."), *ResearchTreeNodeClass->GetName(), *SchematicDep->GetName());
					SubSystem->HandleResearchTreeNodeChange(Node, Schematic.ResearchTree, SchematicClass, SchematicDep);
				}
			} else {
				UE_LOG(LogContentLib, Error, TEXT("Could not load class BPD_ResearchTreeNode_C, something is really wrong"));
			}
		}
	}
}

void UCLSchematicBPFLib::ApplyVisualKitToSchematic(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit, TSubclassOf<UFGSchematic> Item)
{
	auto Obj = Item.GetDefaultObject();
	if (!IsValid(Obj)) {
		return;
	}

	if (IsValid(Subsystem) && Subsystem->Icons.Contains(Kit.BigIcon)) {
		Obj->mSchematicIcon.SetResourceObject(*Subsystem->Icons.Find(Kit.BigIcon));
	} else if (Kit.GetBigIcon()) {
		Obj->mSchematicIcon.SetResourceObject(Kit.GetBigIcon());
	}

	if (IsValid(Subsystem) && Subsystem->Icons.Contains(Kit.SmallIcon)) {
		Obj->mSmallSchematicIcon = *Subsystem->Icons.Find(Kit.SmallIcon);
	} else if (Kit.GetSmallIcon()) {
		Obj->mSmallSchematicIcon = Kit.GetSmallIcon();
	}
}

FString UCLSchematicBPFLib::SerializeSchematic(TSubclassOf<UFGSchematic> Schematic)
{
	const auto CDO = Cast<UFGSchematic>(Schematic->GetDefaultObject());
	if (!IsValid(CDO)) {
		return "";
	}

	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto Desc = MakeShared<FJsonValueString>(CDO->mDescription.ToString());
	const auto Category = MakeShared<FJsonValueString>(CDO->mSchematicCategory ? CDO->mSchematicCategory->GetPathName() : "");
	const auto IconBig = MakeShared<FJsonValueString>(CDO->mSchematicIcon.GetResourceObject() ? CDO->mSchematicIcon.GetResourceObject()->GetPathName() : "");
	const auto IconSmall = MakeShared<FJsonValueString>(CDO->mSmallSchematicIcon ? CDO->mSmallSchematicIcon->GetPathName() : "");

	const auto MenuPriority = MakeShared<FJsonValueNumber>(CDO->mMenuPriority);
	const auto Tier = MakeShared<FJsonValueNumber>(CDO->mTechTier);
	const auto Time = MakeShared<FJsonValueNumber>(CDO->mTimeToComplete);

	TArray<TSharedPtr<FJsonValue>> Cost;
	TArray<TSharedPtr<FJsonValue>> SubCats;
	for (auto& i : CDO->mCost) {
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item", MakeShared<FJsonValueString>(i.ItemClass->GetName()));
		IngObj->Values.Add("Amount", MakeShared<FJsonValueNumber>(i.Amount));
		Cost.Add(MakeShared<FJsonValueObject>(IngObj));
	}

	for (TSubclassOf<UFGSchematicCategory> i : CDO->mSubCategories) {
		SubCats.Add(MakeShared<FJsonValueString>(i->GetPathName()));
	}
	TArray<TSharedPtr<FJsonValue>> Recipes;
	TArray<TSharedPtr<FJsonValue>> Schematics;
	for (const auto i : CDO->mUnlocks) {
		if (const auto UnlockRecipe = Cast<UFGUnlockRecipe>(i)) {
			for (TSubclassOf<UFGRecipe> e : UnlockRecipe->mRecipes) {
				Recipes.Add(MakeShared<FJsonValueString>(e->GetPathName()));
			}
		}
		if (const auto UnlockSchematic = Cast<UFGUnlockSchematic>(i)) {
			for (TSubclassOf<UFGSchematic> e : UnlockSchematic->mSchematics) {
				Schematics.Add(MakeShared<FJsonValueString>(e->GetPathName()));
			}
		}
		// TODO arm slot, inventory slot unlocks
		// TODO info only unlocks
	}
	TArray<TSharedPtr<FJsonValue>> Deps;

	for (auto i : CDO->mSchematicDependencies) {
		if (const auto PurchasedDependency = Cast<UFGSchematicPurchasedDependency>(i)) {
			for (TSubclassOf<UFGSchematic> e : PurchasedDependency->mSchematics) {
				Deps.Add(MakeShared<FJsonValueString>(e->GetPathName()));
			}
		}
	}

	const auto DepArray = MakeShared<FJsonValueArray>(Deps);
	const auto RecipesArray = MakeShared<FJsonValueArray>(Recipes);
	const auto SchematicsArray = MakeShared<FJsonValueArray>(Schematics);
	const auto CostArray = MakeShared<FJsonValueArray>(Cost);
	const auto SubCatArray = MakeShared<FJsonValueArray>(SubCats);

	Obj->Values.Add("Name", Name);
	Obj->Values.Add("Description", Desc);
	Obj->Values.Add("Category", Category);
	Obj->Values.Add("SubCategories", SubCatArray);
	Obj->Values.Add("Costs", CostArray);
	Obj->Values.Add("Time", Time);
	Obj->Values.Add("Tier", Tier);
	Obj->Values.Add("MenuPriority", MenuPriority);
	Obj->Values.Add("IconBig", IconBig);
	Obj->Values.Add("IconSmall", IconSmall);
	Obj->Values.Add("Recipes", RecipesArray);
	Obj->Values.Add("Schematics", SchematicsArray);
	Obj->Values.Add("DependsOn", DepArray);

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UCLSchematicBPFLib::SerializeCLSchematic(FContentLib_Schematic Schematic)
{
	//TODO:: wörk wörk
	return "TODO SerializeCLSchematic";
}
