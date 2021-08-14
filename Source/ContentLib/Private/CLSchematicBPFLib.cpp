


#include "CLSchematicBPFLib.h"

#include "FGSchematicCategory.h"
#include "BPFContentLib.h"
#include "FGResearchTree.h"
#include "AvailabilityDependencies/FGSchematicPurchasedDependency.h"
#include "Reflection/ReflectionHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Unlocks/FGUnlockRecipe.h"



FContentLib_ResearchNode UCLSchematicBPFLib::GenerateResearchStructFromString(FString String)
{
	if(String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_ResearchNode();
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid()) 
		return FContentLib_ResearchNode();
	FContentLib_ResearchNode NodeStruct;

	
	UBPFContentLib::SetStringFieldWithLog(NodeStruct.ResearchTree,"ResearchTree",Result);

	
	for(auto i : Result->TryGetField("Parents")->AsArray())
	{
		if(i->Type == EJson::Object)
		{
			if(i->AsObject()->HasField("X") && i->AsObject()->HasField("Y"))
			{
				FContentLib_Vector2D Vector2D;

				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X,"X",Result);
				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y,"Y",Result);
				NodeStruct.Parents.Add(Vector2D);
			}
		}
	}

	for(auto i : Result->TryGetField("Children")->AsArray())
	{
		if(i->Type == EJson::Object)
		{
			if(i->AsObject()->HasField("Child") && i->AsObject()->HasField("Roads"))
			{
				FContentLib_ResearchNodeRoads Node;
				FContentLib_Vector2D Key;
				if(i->AsObject()->TryGetField("Child")->Type == EJson::Object)
				{
					if(i->AsObject()->TryGetField("Child")->AsObject()->HasField("X") && i->AsObject()->TryGetField("Child")->AsObject()->TryGetField("X")->Type == EJson::Number  &&
					   i->AsObject()->TryGetField("Child")->AsObject()->HasField("Y") && i->AsObject()->TryGetField("Child")->AsObject()->TryGetField("Y")->Type == EJson::Number)
					{
						UBPFContentLib::SetIntegerFieldWithLog(Key.X,"X",i->AsObject()->TryGetField("Child")->AsObject());
						UBPFContentLib::SetIntegerFieldWithLog(Key.Y,"Y",i->AsObject()->TryGetField("Child")->AsObject());
					}
				}
				Node.ChildNode = Key;
				
				TArray<FContentLib_Vector2D> Values;
				for(auto e : i->AsObject()->TryGetField("Roads")->AsArray())
				{
					if(e->Type == EJson::Object)
					{
						if(e->AsObject()->HasField("X") && e->AsObject()->HasField("Y"))
						{
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X,"X",Result);
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y,"Y",Result);
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
	if(String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_ResearchNodeRoads();
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid()) 
		return FContentLib_ResearchNodeRoads();
	FContentLib_ResearchNodeRoads Roads;

	
	for(auto i : Result->TryGetField("Roads")->AsArray())
	{
		if(i->Type == EJson::Object)
		{
			if(i->AsObject()->HasField("X") && i->AsObject()->HasField("Y"))
			{
				FContentLib_Vector2D Vector2D;

				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X,"X", i->AsObject());
				UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y,"Y", i->AsObject());
				Roads.Roads.Add(Vector2D);
			}
		}
	}
	return Roads;
}



FContentLib_Vector2D UCLSchematicBPFLib::GenerateVector2DFromString(FString String)
{
	if(String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_Vector2D();
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid()) 
		return FContentLib_Vector2D();

	FContentLib_Vector2D Vector2D;
	UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X,"X",Result);
	UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y,"Y",Result);
	return Vector2D;
}

FContentLib_Schematic UCLSchematicBPFLib::GenerateCLSchematicFromString(FString String)
{
 	if(String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
		return FContentLib_Schematic();
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid())
		return FContentLib_Schematic();

	FContentLib_Schematic Schematic;

	UBPFContentLib::SetStringFieldWithLog(Schematic.Type,"Type",Result);
	UBPFContentLib::SetStringFieldWithLog(Schematic.Name,"Name",Result);
	UBPFContentLib::SetStringFieldWithLog(Schematic.VisualKit,"VisualKit",Result);
	UBPFContentLib::SetStringFieldWithLog(Schematic.Description,"Description",Result);
	UBPFContentLib::SetStringFieldWithLog(Schematic.Cat,"Cat",Result);
	UBPFContentLib::SetFloatFieldWithLog(Schematic.MenuPriority,"MenuPriority",Result);
	UBPFContentLib::SetFloatFieldWithLog(Schematic.Time,"Time",Result);
	UBPFContentLib::SetIntegerFieldWithLog(Schematic.Tier,"Tier",Result);
	UBPFContentLib::SetIntegerFieldWithLog(Schematic.SlotsToGive,"SlotsToGive",Result);
	UBPFContentLib::SetIntegerFieldWithLog(Schematic.ArmSlotsToGive,"ArmSlotsToGive",Result);

	if(!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.SubCat,"SubCat",Result))
		Schematic.ClearCats = false;
	if(!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.Recipes,"Recipes",Result))
		Schematic.ClearRecipes = false;
	if(!UBPFContentLib::SetStringArrayFieldWithLog(Schematic.DependsOn,"DependsOn",Result))
		Schematic.ClearDeps = false;
	if(!UBPFContentLib::SetStringIntMapFieldWithLog(Schematic.Cost,"Cost",Result))
		Schematic.ClearCost = false;
	if(!UBPFContentLib::SetStringIntMapFieldWithLog(Schematic.ItemsToGive,"ItemsToGive",Result))
		Schematic.ClearItemsToGive = false;
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearCats,"ClearCats",Result);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearCost,"ClearCost",Result);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearRecipes,"ClearRecipes",Result);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearDeps,"ClearDeps",Result);
	UBPFContentLib::SetBooleanFieldWithLog(Schematic.ClearItemsToGive,"ClearItemsToGive",Result);


	if(Result->HasField("ResearchTree"))
	{
		const auto TResult =Result->TryGetField("ResearchTree");
		UBPFContentLib::SetStringFieldWithLog(Schematic.ResearchTree.ResearchTree,"ResearchTree",TResult->AsObject());

		if(TResult->Type == EJson::Object)
		{
			const auto TTResult = TResult->AsObject()->TryGetField("Coords");

			if (TTResult->Type == EJson::Object)
			{
				if (TTResult->AsObject()->HasField("X") && TTResult->AsObject()->HasField("Y"))
				{
					FContentLib_Vector2D Vector2D;

					UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", TTResult->AsObject());
					UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", TTResult->AsObject());
					Schematic.ResearchTree.Coordinates = Vector2D;
				}
			}
			if (TResult->AsObject()->HasField("Parents") && TResult->AsObject()->TryGetField("Parents")->Type == EJson::Array)
			{
				for (auto i : TResult->AsObject()->TryGetField("Parents")->AsArray())
				{
					if (i->Type == EJson::Object)
					{
						if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y"))
						{
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
							Schematic.ResearchTree.Parents.Add(Vector2D);
						}
					}
				}
			}
			if (TResult->AsObject()->HasField("UnHiddenBy") && TResult->AsObject()->TryGetField("UnHiddenBy")->Type == EJson::Array)
			{
				for (auto i : TResult->AsObject()->TryGetField("UnHiddenBy")->AsArray())
				{
					if (i->Type == EJson::Object)
					{
						if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y"))
						{
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
							Schematic.ResearchTree.UnHiddenBy.Add(Vector2D);
						}
					}
				}
			}

			if (TResult->AsObject()->HasField("NodesToUnHide") && TResult->AsObject()->TryGetField("NodesToUnHide")->Type == EJson::Array)
			{
				for (auto i : TResult->AsObject()->TryGetField("NodesToUnHide")->AsArray())
				{
					if (i->Type == EJson::Object)
					{
						if (i->AsObject()->HasField("X") && i->AsObject()->HasField("Y"))
						{
							FContentLib_Vector2D Vector2D;

							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.X, "X", i->AsObject());
							UBPFContentLib::SetIntegerFieldWithLog(Vector2D.Y, "Y", i->AsObject());
							Schematic.ResearchTree.NodesToUnHide.Add(Vector2D);
						}
					}
				}
			}
			if (TResult->AsObject()->HasField("Children") && TResult->AsObject()->TryGetField("Children")->Type == EJson::Array)
			{
				for (auto i : TResult->AsObject()->TryGetField("Children")->AsArray())
				{
					if (i->Type == EJson::Object)
					{
						if (i->AsObject()->HasField("ChildNode") && i->AsObject()->HasField("Roads"))
						{
							FContentLib_Vector2D Key;
							if (i->AsObject()->TryGetField("ChildNode")->Type == EJson::Object)
							{
								UBPFContentLib::SetIntegerFieldWithLog(Key.X, "X", i->AsObject()->TryGetField("ChildNode")->AsObject());
								UBPFContentLib::SetIntegerFieldWithLog(Key.Y, "Y", i->AsObject()->TryGetField("ChildNode")->AsObject());
							}

							TArray<FContentLib_Vector2D> Values;
							for (auto e : i->AsObject()->TryGetField("Roads")->AsArray())
							{
								if (e->Type == EJson::Object)
								{
								
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

void UCLSchematicBPFLib::InitSchematicFromStruct(FContentLib_Schematic Schematic,TSubclassOf<UFGSchematic> SchematicClass,UContentLibSubsystem *SubSystem)
{
	UFGSchematic * CDO = SchematicClass.GetDefaultObject();

	if(Schematic.Type != "")
	{
		if(Schematic.Type.Equals("Custom",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Custom;
		else if(Schematic.Type.Equals("Cheat",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Cheat;
		else if(Schematic.Type.Equals("Tutorial",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Tutorial;
		else if(Schematic.Type.Equals("Milestone",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Milestone;
		else if(Schematic.Type.Equals("Alternate",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Alternate;
		else if(Schematic.Type.Equals("Story",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Story;
		else if(Schematic.Type.Equals("MAM",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_MAM;
		else if(Schematic.Type.Equals("ResourceSink",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_ResourceSink;
		else if(Schematic.Type.Equals("HardDrive",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_HardDrive;
		else if(Schematic.Type.Equals("Prototype",ESearchCase::IgnoreCase))
			CDO->mType = ESchematicType::EST_Prototype;
	}
		
	
	if(Schematic.Name != "")
		CDO->mDisplayName = FText::FromString(Schematic.Name);

	if(Schematic.Description != "")
		CDO->mDescription = FText::FromString(Schematic.Description);
	
	if(Schematic.Cat != "")
	{
		TSubclassOf<UFGSchematicCategory> Out = UBPFContentLib::SetCategoryWithLoad(Schematic.Cat, SubSystem, true);
		if(Out)
			CDO->mSchematicCategory = Out;
	}

	if(Schematic.SubCat.Num() > 0)
	{
		CDO->mSubCategories.Empty();
		for(auto i : Schematic.SubCat)
		{
			TSubclassOf<UFGSchematicCategory> Out = UBPFContentLib::SetCategoryWithLoad(i, SubSystem, true);
			if(Out)
				CDO->mSubCategories.Add(Out);
		}
	}

	if(Schematic.MenuPriority != -1)
		CDO->mMenuPriority = Schematic.MenuPriority;

	if(Schematic.Tier != -1)
		CDO->mTechTier = Schematic.Tier;
	
	UBPFContentLib::AddToItemAmountArray(CDO->mCost,Schematic.Cost,SubSystem->mItems,Schematic.ClearCost);
	
	if(Schematic.Time != -1)
		CDO->mTimeToComplete = Schematic.Time;

	if(Schematic.ClearRecipes)
	{
		auto copy = CDO->mUnlocks;
		for(auto i : copy)
		{
			if(Cast<UFGUnlockRecipe>(i))
			{
				CDO->mUnlocks.Remove(i);
			}
		}
	}
	for(FString i : Schematic.Recipes)
	{
		UClass * RecipesClass = UBPFContentLib::FindClassWithLog(i,UFGRecipe::StaticClass(),SubSystem);
		if (RecipesClass)
		{
			UBPFContentLib::AddRecipeToUnlock(SchematicClass, SubSystem, RecipesClass);
		}
	}

	for(FString i : Schematic.Schematics)
	{
		UClass * Class = UBPFContentLib::FindClassWithLog(i,UFGSchematic::StaticClass(),SubSystem);
		if (Class)
		{
			UBPFContentLib::AddSchematicToUnlock(SchematicClass, SubSystem, Class);
		}
	}

	
	if(Schematic.ItemsToGive.Num() || Schematic.ClearItemsToGive)
	{
		UBPFContentLib::AddGiveItemsToUnlock(SchematicClass, SubSystem, Schematic.ItemsToGive,Schematic.ClearItemsToGive);
	}
	

	if (Schematic.SlotsToGive > -1)
	{
		UBPFContentLib::AddSlotsToUnlock(SchematicClass, SubSystem, Schematic.SlotsToGive);
	}
	
	if (Schematic.ArmSlotsToGive > -1)
	{
		UBPFContentLib::AddArmSlotsToUnlock(SchematicClass, SubSystem, Schematic.ArmSlotsToGive);
	}
	
	for(FString i : Schematic.DependsOn)
	{
		UClass * SchematicDep = UBPFContentLib::FindClassWithLog(i,UFGSchematic::StaticClass(),SubSystem);
		if (SchematicDep && SchematicDep->IsChildOf(UFGSchematic::StaticClass()))
		{
			UBPFContentLib::AddSchematicToPurchaseDep(SchematicClass,SubSystem,SchematicDep);
		}
	}

	if(Schematic.VisualKit != "")
	{
		if (Schematic.VisualKit.Contains("{") && Schematic.VisualKit.Contains("}"))
		{
			const FContentLib_VisualKit Kit = UCLItemBPFLib::GenerateKitFromString(Schematic.VisualKit);
			ApplyVisualKitToSchematic(SubSystem, Kit, SchematicClass);
		}
		else
		{
			if (SubSystem->ImportedVisualKits.Contains(Schematic.VisualKit))
				ApplyVisualKitToSchematic(SubSystem, *SubSystem->ImportedVisualKits.Find(Schematic.VisualKit), SchematicClass);
			else if (SubSystem->VisualKits.Contains(Schematic.VisualKit))
				ApplyVisualKitToSchematic(SubSystem, *SubSystem->VisualKits.Find(Schematic.VisualKit), SchematicClass);
		}
		
	}
	if (Schematic.ResearchTree.ResearchTree !=  "")
	{
		UClass* SchematicDep = UBPFContentLib::FindClassWithLog(Schematic.ResearchTree.ResearchTree, UFGResearchTree::StaticClass(), SubSystem);
		if(SchematicDep && SchematicDep->IsChildOf(UFGResearchTree::StaticClass()))
		{
			static FStructProperty* NodeDataStructProperty = nullptr;
			static FClassProperty* SchematicStructProperty = nullptr;
			static UClass* ResearchTreeNodeClass = nullptr;

			ResearchTreeNodeClass = LoadClass<UFGResearchTreeNode>(
				nullptr, TEXT("/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"));
			if (ResearchTreeNodeClass)
			{
				NodeDataStructProperty = FReflectionHelper::FindPropertyChecked<FStructProperty>(
					ResearchTreeNodeClass, TEXT("mNodeDataStruct"));
				SchematicStructProperty = FReflectionHelper::FindPropertyByShortNameChecked<FClassProperty>(
					NodeDataStructProperty->Struct, TEXT("Schematic"));
				TArray<UFGResearchTreeNode*> & Nodes = Cast<UFGResearchTree>(SchematicDep->GetDefaultObject())->mNodes;
				bool Found = false;
				for (UFGResearchTreeNode* Node : Nodes)
				{
					if (!Node)
						continue;
					TSubclassOf<UFGSchematic> mSchematic = Cast<UClass>(
						SchematicStructProperty->GetPropertyValue_InContainer(
							NodeDataStructProperty->ContainerPtrToValuePtr<void>(Node)));
					if (!mSchematic || mSchematic != SchematicClass)
						continue;
					Found = true;
					SubSystem->HandleResearchTreeNodeChange(Node, Schematic.ResearchTree, SchematicClass,SchematicDep);
				}
				if (!Found)
				{
					
					
					UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BPD_ResearchTreeNode"), false);
					if (!Class)
					{
						//Blueprint'/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode'
						Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"));
						if (!Class)
						{
							UE_LOG(LogTemp, Fatal, TEXT("CL: Couldnt find /Game/FactoryGame/Schematics/Research/BPD_ResearchTreeNode.BPD_ResearchTreeNode_C"))
						}
					}
					UFGResearchTreeNode* Node = NewObject<UFGResearchTreeNode>(SchematicDep->GetDefaultObject(), Class);
					Nodes.Add(Node);
					UE_LOG(LogTemp, Warning, TEXT("CL: Created new Research Tree Node. Added Schematic to %s in ResearchTree %s."), *ResearchTreeNodeClass->GetName(), *SchematicDep->GetName());
					SubSystem->HandleResearchTreeNodeChange(Node, Schematic.ResearchTree, SchematicClass,SchematicDep);

				}

			}
		}
	}

	
}



void UCLSchematicBPFLib::ApplyVisualKitToSchematic(UContentLibSubsystem* Subsystem,FContentLib_VisualKit Kit, TSubclassOf<UFGSchematic> Item)
{
	if(!Item)
		return;
	auto* Obj = Item.GetDefaultObject();
	
	if(Subsystem)
	{
		if(Subsystem->Icons.Contains(Kit.BigIcon))
		{
			Obj->mSchematicIcon.SetResourceObject(*Subsystem->Icons.Find(Kit.BigIcon));
		}
		else
		{
			if (Kit.GetBigIcon())
				Obj->mSchematicIcon.SetResourceObject(Kit.GetBigIcon());
		}
	}
	else
	{
		if (Kit.GetBigIcon())
			Obj->mSchematicIcon.SetResourceObject(Kit.GetBigIcon());
	}
	
	if(Subsystem)
	{
		if(Subsystem->Icons.Contains(Kit.SmallIcon))
		{
			Obj->mSmallSchematicIcon = *Subsystem->Icons.Find(Kit.SmallIcon);
		}
		else
		{
			if (Kit.GetSmallIcon())
				Obj->mSmallSchematicIcon = Kit.GetSmallIcon();
		}
	}
	else
	{
		if (Kit.GetSmallIcon())
			Obj->mSmallSchematicIcon = Kit.GetSmallIcon();
	}
}

FString UCLSchematicBPFLib::SerializeSchematic(TSubclassOf<UFGSchematic> Schematic)
{

	const auto CDO = Cast<UFGSchematic>(Schematic->GetDefaultObject());
	const auto Obj = MakeShared<FJsonObject>();
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto Desc = MakeShared<FJsonValueString>(CDO->mDescription.ToString());
	const auto Cat = MakeShared<FJsonValueString>(CDO->mSchematicCategory ? CDO->mSchematicCategory->GetPathName() : "");
	const auto IconBig = MakeShared<FJsonValueString>(CDO->mSchematicIcon.GetResourceObject() ? CDO->mSchematicIcon.GetResourceObject()->GetPathName() : "");
	const auto IconSmall = MakeShared<FJsonValueString>(CDO->mSmallSchematicIcon ? CDO->mSmallSchematicIcon->GetPathName() : "");

	const auto MenuPriority = MakeShared<FJsonValueNumber>(CDO->mMenuPriority);
	const auto Tier = MakeShared<FJsonValueNumber>(CDO->mTechTier);
	const auto Time = MakeShared<FJsonValueNumber>(CDO->mTimeToComplete);

	TArray< TSharedPtr<FJsonValue>> Cost;
	TArray< TSharedPtr<FJsonValue>> SubCats; 
	for(auto i : CDO->mCost)
	{
		auto IngObj = MakeShared<FJsonObject>();
		IngObj->Values.Add("Item",MakeShared<FJsonValueString>(i.ItemClass->GetName()));
		IngObj->Values.Add("Amount",MakeShared<FJsonValueNumber>(i.Amount));
		Cost.Add(MakeShared<FJsonValueObject>(IngObj));
	}
	
	for(auto i : CDO->mSubCategories)
	{
		auto IngObj = MakeShared<FJsonObject>();
		SubCats.Add(MakeShared<FJsonValueString>(i->GetPathName()));
	}
	TArray< TSharedPtr<FJsonValue>> Recipes;
	for(auto i : CDO->mUnlocks)
	{
		if(Cast<UFGUnlockRecipe>(i))
		{
			for(auto e: Cast<UFGUnlockRecipe>(i)->mRecipes)
			{
				auto IngObj = MakeShared<FJsonObject>();
				Recipes.Add(MakeShared<FJsonValueString>(e->GetPathName()));
			}
		}
	}
	TArray< TSharedPtr<FJsonValue>> Deps;

	for(auto i : CDO->mSchematicDependencies)
	{
		if(Cast<UFGSchematicPurchasedDependency>(i))
		{
			for(auto e: Cast<UFGSchematicPurchasedDependency>(i)->mSchematics)
			{
				auto IngObj = MakeShared<FJsonObject>();
				Deps.Add(MakeShared<FJsonValueString>(e->GetPathName()));
			}
		}
	}
	const auto DepArray = MakeShared<FJsonValueArray>(Deps);

	const auto RecipesArray = MakeShared<FJsonValueArray>(Recipes);

	const auto CostArray = MakeShared<FJsonValueArray>(Cost);
	const auto SubCatArray = MakeShared<FJsonValueArray>(SubCats);

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("Description", Desc);
	Obj->Values.Add("Cat", Cat);
	Obj->Values.Add("SubCat", SubCatArray);
	Obj->Values.Add("Costs",CostArray);
	Obj->Values.Add("Time",Time);
	Obj->Values.Add("Tier",Tier);
	Obj->Values.Add("MenuPriority",MenuPriority);
	Obj->Values.Add("IconBig",IconBig);
	Obj->Values.Add("IconSmall",IconSmall);
	Obj->Values.Add("Recipes",RecipesArray);
	Obj->Values.Add("DependsOn",DepArray);
	
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UCLSchematicBPFLib::SerializeCLSchematic(FContentLib_Schematic Schematic)
{
	return "";
};


