


#include "BPFContentLib.h"



#include "FGItemCategory.h"
#include "FGSchematic.h"
#include "FGSchematicCategory.h"
#include "FGSchematicManager.h"
#include "FGUnlockSubsystem.h"
#include "FGWorkBench.h"
#include "FGResearchTree.h"
#include "AvailabilityDependencies/FGSchematicPurchasedDependency.h"
#include "Patching/BlueprintHookHelper.h"
#include "Patching/BlueprintHookManager.h"
#include "Resources/FGBuildingDescriptor.h"
#include "Resources/FGItemDescriptor.h"
#include "Unlocks/FGUnlockArmEquipmentSlot.h"
#include "Unlocks/FGUnlockGiveItem.h"
#include "Unlocks/FGUnlockInventorySlot.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "UObject/UObjectAllocator.h"
#include "Util/ImageLoadingUtil.h"


TArray<FString> UBPFContentLib::GetBlueprintFunctionNames(UClass* BlueprintClass)
{
	TArray<FString> Names;

	if(BlueprintClass)
		for (auto i : BlueprintClass->*get(grab_FuncTable()))
			Names.Add(i.Key.ToString());
	return Names;
}


UObject* UBPFContentLib::Conv_ClassToObject(UClass* Class)
{
	if(Class)
		return Class->GetDefaultObject();
	return nullptr;
}

void UBPFContentLib::BindOnBPFunction(const TSubclassOf<UObject> Class, FObjectFunctionBind Binding, const FString FunctionName)
{
	if(!Class)
		return;
	UFunction* ConstructFunction = Class->FindFunctionByName(*FunctionName);
	if (!ConstructFunction || ConstructFunction->IsNative())
	{
		if (!ConstructFunction)
		{
			UE_LOG(LogTemp, Error, TEXT("Was not able to Bind on Function : %s Function was not Found. Function Dump:"), *FunctionName);
			for (auto i : Class->*get(grab_FuncTable()))
			{
				UE_LOG(LogTemp, Error, TEXT("FunctionName : %s"), *i.Key.ToString())
			}
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Was not able to Bind on Function : %s Function was Native"), *FunctionName);

		return;
	}
	UBlueprintHookManager* HookManager = GEngine->GetEngineSubsystem<UBlueprintHookManager>();
	HookManager->HookBlueprintFunction(ConstructFunction, [Binding](FBlueprintHookHelper& HookHelper) {
        Binding.ExecuteIfBound(HookHelper.GetContext());
    }, EPredefinedHookOffset::Return);
}

UTexture2D* UBPFContentLib::GetIconForBuilding(UContentLibSubsystem* System, TSubclassOf<AFGBuildable> Buildable, bool Big, const UObject* WorldContext)
{
	TArray<TSubclassOf<AFGBuildable>> Arr;
	if(System->BuildGunBuildings.Contains(Buildable))
	{
		const TSubclassOf<class UFGBuildingDescriptor> Desc = *System->BuildGunBuildings.Find(Buildable);
		if(Big)
			return Desc.GetDefaultObject()->GetBigIcon(Desc);
		else
			return Desc.GetDefaultObject()->GetSmallIcon(Desc);
	}
	return nullptr;
}



bool UBPFContentLib::SetStringIntMapFieldWithLog(TMap<FString,int32>& Field, FString FieldName, TSharedPtr<FJsonObject> Result)
{
	if(!Result->HasField(FieldName))
		return false;

	if(Result->TryGetField(FieldName)->Type != EJson::Array)
		return false;

	for(auto i : Result->TryGetField(FieldName)->AsArray())
	{
		if(i->Type == EJson::Object)
		{
			const auto Obj = i->AsObject();
			const bool HasItem = Obj->HasField("Item");
			const bool HasAmount = Obj->HasField("Amount");
			if (HasItem && HasAmount)
			{
				const TSharedPtr<FJsonValue> Item = Obj->TryGetField("Item");
				const TSharedPtr<FJsonValue> Amount = Obj->TryGetField("Amount");

				if (Item->Type == EJson::String && Amount->Type == EJson::Number)
				{
					Field.Add(Item->AsString(), Amount->AsNumber());
				}
				else
				{
					if (Item->Type != EJson::String)
					{
						UE_LOG(LogTemp, Error, TEXT("Invalid Type on Ingredient. Expected String"));
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Invalid Type on Amount! Expected Number"));
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No Item or Amount In Json"));
			}
		}
	}
	return true;
}
bool UBPFContentLib::SetColorFieldWithLog(FColor& Field, FString FieldName, TSharedPtr<FJsonObject> Result)
{
	if (Result->HasField(FieldName) && Result->TryGetField(FieldName)->Type == EJson::Object)
	{
		const auto Obj = Result->TryGetField(FieldName)->AsObject();
		if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a"))
		{
			if (Obj->TryGetField("r")->Type == EJson::Number)
			{
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.R, "r", Obj);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("R value in Color is not of type Number !"));
			}
			if (Obj->TryGetField("g")->Type == EJson::Number)
			{
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.G, "g", Obj);

			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("G value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("b")->Type == EJson::Number)
			{
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.B, "b", Obj);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("b value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("a")->Type == EJson::Number)
			{
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.A, "a", Obj);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("A value in Color is not of type Number !"));
			}
		}
		return true;
	}
	return false;
}
bool UBPFContentLib::SetLinearColorFieldWithLog(FLinearColor& Field, FString FieldName, TSharedPtr<FJsonObject> Result)
{

	if (Result->HasField(FieldName) && Result->TryGetField(FieldName)->Type == EJson::Object)
	{
		const auto Obj = Result->TryGetField(FieldName)->AsObject();
		if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a"))
		{
			if (Obj->TryGetField("r")->Type == EJson::Number)
			{
				UBPFContentLib::SetFloatFieldWithLog(Field.R, "r", Obj);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("R value in Color is not of type Number !"));
			}
			if (Obj->TryGetField("g")->Type == EJson::Number)
			{
				UBPFContentLib::SetFloatFieldWithLog(Field.G, "g", Obj);

			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("G value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("b")->Type == EJson::Number)
			{
				UBPFContentLib::SetFloatFieldWithLog(Field.B, "b", Obj);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("b value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("a")->Type == EJson::Number)
			{
				UBPFContentLib::SetFloatFieldWithLog(Field.A, "a", Obj);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("A value in Color is not of type Number !"));
			}
		}
		return true;
	}
	return false;
}

void UBPFContentLib::SetBooleanFieldWithLog(bool& Field, const FString FieldName, TSharedPtr<FJsonObject> Result)
{
	if(!Result->HasField(FieldName))
	return;

	if(Result->TryGetField(FieldName)->Type != EJson::Boolean)
	{
		UE_LOG(LogTemp,Error,TEXT("Field %s is not of Type Boolean"), *FieldName)
		return;
	}
	
	Field = Result->TryGetField(FieldName)->AsBool();
	
}
void UBPFContentLib::SetFloatFieldWithLog(float& Field, const FString FieldName, TSharedPtr<FJsonObject> Result)
{
	if(!Result->HasField(FieldName))
		return;

	if(Result->TryGetField(FieldName)->Type != EJson::Number)
	{
		UE_LOG(LogTemp,Error,TEXT("Field %s is not of Type Number"), *FieldName)
		return;
	}
	
	Field = Result->TryGetField(FieldName)->AsNumber();
}

void UBPFContentLib::SetIntegerFieldWithLog(int32 & Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if(!Result->HasField(FieldName))
		return;

	if(Result->TryGetField(FieldName)->Type != EJson::Number && Result->TryGetField(FieldName)->Type != EJson::Boolean)
	{
		UE_LOG(LogTemp,Error,TEXT("Field %s is not of Type Number or Boolean"), *FieldName)
		return;
	}
	else if (Result->TryGetField(FieldName)->Type == EJson::Boolean)
	{
		Field = static_cast<int32>(Result->TryGetField(FieldName)->AsBool());
		return;
	}
	
	Field = Result->TryGetField(FieldName)->AsNumber();
}

void UBPFContentLib::SetSmallIntegerFieldWithLog(uint8& Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName))
		return;

	if (Result->TryGetField(FieldName)->Type != EJson::Number && Result->TryGetField(FieldName)->Type != EJson::Boolean)
	{
		UE_LOG(LogTemp, Error, TEXT("Field %s is not of Type Number or Boolean"), *FieldName)
			return;
	}
	else if (Result->TryGetField(FieldName)->Type == EJson::Boolean)
	{
		Field = static_cast<uint8>(Result->TryGetField(FieldName)->AsBool());
		return;
	}

	Field = Result->TryGetField(FieldName)->AsNumber();
}
void UBPFContentLib::SetStringFieldWithLog(FString & Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if(!Result->HasField(FieldName))
		return;

	if(Result->TryGetField(FieldName)->Type != EJson::String)
	{
		UE_LOG(LogTemp,Error,TEXT("Field %s is not of Type String"), *FieldName)
		return;
	}
	
	Field = Result->TryGetField(FieldName)->AsString();
}

bool UBPFContentLib::SetStringArrayFieldWithLog(TArray<FString>& Field, FString FieldName,TSharedPtr<FJsonObject> Result)
{
	if(!Result->HasField(FieldName))
		return false;

	if(Result->TryGetField(FieldName)->Type != EJson::Array)
	{
		UE_LOG(LogTemp,Error,TEXT("Field %s is not of Type Array"), *FieldName)
		return false;
	}
	for(auto i : Result->TryGetField(FieldName)->AsArray())
    {
    	if(i->Type == EJson::String)
    	{
    		FString Item = i->AsString();
    		if(Item != "")
    		{
    			if(!Field.Contains((Item)))
    				Field.Add(Item);
    		}
    	}
		else
		{
			UE_LOG(LogTemp,Error,TEXT("Field %s contains Elements of invalid Type"), *FieldName)
		}
    }
	return true;
}

void UBPFContentLib::WriteStringToFile(FString Path, FString resultString, bool Relative) {
#if WITH_EDITOR 
	FFileHelper::SaveStringToFile(resultString, Relative ? *(FPaths::ProjectDir() + Path) : *Path);

#else
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = AbsoluteRootPath + TEXT("Mods/") + Path;
	if (!AbsolutePath.Contains(TEXT("..")))
	{
		FFileHelper::SaveStringToFile(resultString, *AbsolutePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
	}
#endif
}
UTexture2D* UBPFContentLib::LoadTextureFromFile(FString& String, FString Path, bool Relative) {
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(Path);
	if (AbsolutePath.StartsWith(AbsoluteRootPath))
	{
		FString OutErrorMessage;
		UTexture2D* LoadedModIcon = FImageLoadingUtil::LoadImageFromFile(*AbsolutePath, OutErrorMessage);
		return LoadedModIcon;	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
		return nullptr;
	}
}



bool UBPFContentLib::LoadStringFromFile(FString& String, FString Path, bool Relative) {
#if WITH_EDITOR 
	return FFileHelper::LoadFileToString(String, Relative ? *(FPaths::ProjectDir() + Path) : *Path);
#else
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(Path);
	if (AbsolutePath.StartsWith(AbsoluteRootPath))
	{
		return FFileHelper::LoadFileToString(String, *AbsolutePath);
	}
	else
	{
		return false;
		UE_LOG(LogTemp, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
	}
#endif
}

bool UBPFContentLib::GetDirectoriesInPath(const FString& FullPathOfBaseDir, TArray<FString>& DirsOut, const FString& NotContainsStr, bool Recursive, const FString& ContainsStr)
{
	FString Str;

	auto FilenamesVisitor = MakeDirectoryVisitor(
		[&](const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			if (bIsDirectory)
			{
				//Using a Contains Filter?
				if (ContainsStr != "")
				{
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);
					//Only if Directory Contains Str		
					if (Str.Contains(ContainsStr))
					{
						if (Recursive) DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
						else DirsOut.Push(Str);
					}

				}
				else if (NotContainsStr != "") {
					if (!Str.Contains(NotContainsStr))
					{
						if (Recursive) DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
						else DirsOut.Push(Str);
					}
				}
				//Get ALL Directories!
				else
				{
					//Just the Directory
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);

					if (Recursive) DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
					else DirsOut.Push(Str);
				}
			}
			return true;
		}
	);
	if (Recursive)
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FullPathOfBaseDir, FilenamesVisitor);
	}
	else
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPathOfBaseDir, FilenamesVisitor);
	}
}

void UBPFContentLib::String_Sort(UPARAM(ref) TArray <FString>& Array_To_Sort, bool Descending, bool FilterToUnique, TArray <FString>& Sorted_Array)
{
	if (FilterToUnique)
	{
		for (auto i : Array_To_Sort)
		{
			if (FilterToUnique)
			{
				if (!Sorted_Array.Contains(i))
				{
					Sorted_Array.Add(i);
				}
			}
		}
	}
	Sorted_Array.Sort();               // Sort array using built in function (sorts A-Z)

	if (Descending == true)
	{
		TArray <FString> NewArray;      // Define "temp" holding array
		int x = Sorted_Array.Num() - 1;

		while (x > -1)
		{
			NewArray.Add(Sorted_Array[x]); // loop through A-Z sorted array and remove element from back and place it in beginning of "temp" array
			--x;
		}

		Sorted_Array = NewArray;   // Set reference array to "temp" array order, array is now Z-A
	}
}

bool UBPFContentLib::GetFilesInPath(const FString& FullPathOfBaseDir, TArray<FString>& FilenamesOut, bool Recursive, const FString& FilterByExtension)
{
	//Format File Extension, remove the "." if present
	const FString FileExt = FilterByExtension.Replace(TEXT("."), TEXT("")).ToLower();

	FString Str;
	auto FilenamesVisitor = MakeDirectoryVisitor(
		[&](const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			//Files
			if (!bIsDirectory)
			{
				//Filter by Extension
				if (FileExt != "")
				{
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);

					//Filter by Extension
					if (FPaths::GetExtension(Str).ToLower() == FileExt)
					{
						if (Recursive)
						{
							FilenamesOut.Push(FilenameOrDirectory); //need whole path for recursive
						}
						else
						{
							FilenamesOut.Push(Str);
						}
					}
				}

				//Include All Filenames!
				else
				{
					//Just the Directory
					Str = FPaths::GetCleanFilename(FilenameOrDirectory);

					if (Recursive)
					{
						FilenamesOut.Push(FilenameOrDirectory); //need whole path for recursive
					}
					else
					{
						FilenamesOut.Push(Str);
					}
				}
			}
			return true;
		}
	);
	if (Recursive)
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FullPathOfBaseDir, FilenamesVisitor);
	}
	else
	{
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPathOfBaseDir, FilenamesVisitor);
	}
}


TSubclassOf<UObject> UBPFContentLib::FindClassWithLog(FString Name, UClass* ParentClass, UContentLibSubsystem* RootFolder)
{
	if (!ParentClass)
		return nullptr;

	if (Name == "")
	{
		UE_LOG(LogTemp, Error, TEXT("Finding %s by Name failed. Empty Path !"), *ParentClass->GetName());
		return nullptr;
	}
	if (Name.Contains("/"))
	{ 
	    UClass* Loaded = LoadObject<UClass>(nullptr, *Name);
	    if (Loaded && Loaded->IsChildOf(ParentClass))
	    {
		    return Loaded;
	    }
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Finding %s by Path %s failed"), *ParentClass->GetName(), *Name);
		}
	}
	else
	{
		if(Name.Contains(" "))
		{
			Name = Name.Replace(TEXT(" "),TEXT("-"),ESearchCase::CaseSensitive);
		}
		FString Prefix; FString Suffix = "_C"; TArray<UClass*> ClassArr;
		if(ParentClass->IsChildOf(UFGItemDescriptor::StaticClass()))
		{
			ClassArr = RootFolder->mItems;
			TArray<TSubclassOf<UFGItemDescriptor>> Arr;
			RootFolder->CreatedItems.GetKeys(Arr);
			ClassArr.Append(Arr);
			Prefix = "Desc_";
		}
		else if (ParentClass->IsChildOf(UFGItemCategory::StaticClass()))
		{
			ClassArr = RootFolder->mItemCategories;
			Prefix = "Cat_";
		}
		else if (ParentClass->IsChildOf(UFGSchematicCategory::StaticClass()))
		{
			ClassArr = RootFolder->mSchematicCategories;
			Prefix = "SC_";
		}
		else if(ParentClass->IsChildOf(UFGSchematic::StaticClass()))
		{
			ClassArr = RootFolder->mSchematics;
			TArray<TSubclassOf<UFGSchematic>> Arr;
			RootFolder->CreatedSchematics.GetKeys(Arr);
			ClassArr.Append(Arr);
			Prefix= "";

		}
		else if(ParentClass->IsChildOf(UFGWorkBench::StaticClass()))
		{
			ClassArr = RootFolder->mCraftingComps;
			Prefix= "";
		}
		else if (ParentClass->IsChildOf(UFGRecipe::StaticClass()))
		{
			ClassArr = RootFolder->mRecipes;
			TArray<TSubclassOf<UFGRecipe>> Arr;
			RootFolder->CreatedRecipes.GetKeys(Arr);
			ClassArr.Append(Arr);
			Prefix = "Recipe_";
		}
		else if (ParentClass->IsChildOf(UFGResearchTree::StaticClass()))
		{
			ClassArr = RootFolder->mResearchTrees;
			Prefix = "";
		}
		else if(ParentClass->IsChildOf(UObject::StaticClass()))
		{
			ClassArr = RootFolder->mBuilders;

			Prefix = "Build_";
		}
		else
			Prefix= "";

		for(auto e : ClassArr)
		{
			FString S = e->GetName();
			FString Tp = Name;
			FString DescPre = FString(Prefix).Append(Name);
			if (!Name.EndsWith(Suffix))
			{
				DescPre.Append(Suffix);
			}
			if(S.Equals(Name,ESearchCase::IgnoreCase) || S.Equals(Tp.Append(Suffix), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
			{
				return e;
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Finding %s by Name %s failed"), *ParentClass->GetName(), *Name);
	return nullptr;
	
}

TSubclassOf<UObject> UBPFContentLib::CreateContentLibClass(FString Name, UClass* ParentClass)
{
	if (Name == "" || FindObject<UClass>(ANY_PACKAGE, *Name, false) || FindObject<UClass>(
		ANY_PACKAGE, *Name.Append("_C"), false))
		return nullptr;
	const EClassFlags ParamsClassFlags = CLASS_Native | CLASS_MatchedSerializers;
	//Code below is taken from GetPrivateStaticClassBody
	//Allocate memory from ObjectAllocator for class object and call class constructor directly
	UClass* ConstructedClassObject = (UClass*)GUObjectAllocator.AllocateUObject(
		sizeof(UDynamicClass), alignof(UDynamicClass), true);
	::new(ConstructedClassObject)UDynamicClass(
		EC_StaticConstructor,
		*Name,
		ParentClass->GetStructureSize(),
		ParentClass->GetMinAlignment(),
		CLASS_Intrinsic,
		CASTCLASS_None,
		UObject::StaticConfigName(),
		EObjectFlags(RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative | RF_MarkAsRootSet),
		ParentClass->ClassConstructor,
		ParentClass->ClassVTableHelperCtorCaller,
		ParentClass->ClassAddReferencedObjects, nullptr);

	//Set super structure and ClassWithin (they are required prior to registering)
	FCppClassTypeInfoStatic TypeInfoStatic = { false };
	ConstructedClassObject->SetSuperStruct(ParentClass);
	ConstructedClassObject->ClassWithin = UObject::StaticClass();
	ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#if WITH_EDITOR
	//Field with cpp type info only exists in editor, in shipping SetCppTypeInfoStatic is empty
	ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#endif
	//Register pending object, apply class flags, set static type info and link it
	ConstructedClassObject->RegisterDependencies();

	ConstructedClassObject->DeferredRegister(UDynamicClass::StaticClass(), TEXT("/ContentLib/"), *Name);

	//Mark class as Constructed and perform linking
	ConstructedClassObject->ClassFlags |= (EClassFlags)(ParamsClassFlags | CLASS_Constructed);
	ConstructedClassObject->AssembleReferenceTokenStream(true);
	ConstructedClassObject->StaticLink();

	//Make sure default class object is initialized
	ConstructedClassObject->GetDefaultObject();
	return ConstructedClassObject;
}


bool UBPFContentLib::StringCompareItem(FString e, FString Name, FString Prefix, FString Suffix)
{
	FString S = e;
	FString A = Name;
	FString DescPre = Prefix.Append(Name);
	if (!DescPre.EndsWith(Suffix))
	{
		DescPre.Append(Suffix);
	}
	if (S.Equals(Name, ESearchCase::IgnoreCase) || S.Equals(A.Append(Suffix), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
	{
		return true;
	}
	return false;
}

UClass* UBPFContentLib::SetCategoryWithLoad(FString CategoryString,UContentLibSubsystem* Subsystem, bool Schematic)
{
	UClass* CategoryClass = nullptr;
	if(CategoryString.Contains(" "))
	{
		CategoryString = CategoryString.Replace(TEXT(" "),TEXT("-"),ESearchCase::CaseSensitive);
	}
	if (CategoryString.Contains("/"))
	{
		UClass* Loaded = LoadObject<UClass>(nullptr, *CategoryString);
		if (Loaded && Loaded->IsChildOf(Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass()))
			CategoryClass = Loaded;
		else
		{
			FString Left;
			FString Right;
			CategoryString.Split("/", &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			if (Right != "")
			{
				FString TempName = "Cat_";
				TempName.Append(Right);
				TSubclassOf<UFGItemCategory> Cat = UBPFContentLib::CreateContentLibClass(CategoryString, Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass());
				if (Cat)
				{
					UE_LOG(LogTemp, Warning, TEXT("CL: Created Category %s"), *Cat->GetName())
					if(Schematic)
					{
						Cast<UFGSchematicCategory>(Cat->GetDefaultObject())->mDisplayName = FText::FromString(Right);
						Subsystem->mSchematicCategories.Add(Cat);
					}
					else
					{
						Subsystem->mItemCategories.Add(Cat);
						Cast<UFGItemCategory>(Cat->GetDefaultObject())->mDisplayName = FText::FromString(Right);
					}	
					CategoryClass = Cat;
				}
				else
				{
					UClass* Find = FindObject<UClass>(ANY_PACKAGE, *TempName, false);
					if (!Find)
					{
						Find = FindObject<UClass>(ANY_PACKAGE, *TempName.Append("_C"), false);
					}
					if (Find && Find->IsChildOf(Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass()))
					{
						if(Schematic)
							Cast<UFGSchematicCategory>(Cat->GetDefaultObject())->mDisplayName = FText::FromString(Right);
						else
							Cast<UFGItemCategory>(Cat->GetDefaultObject())->mDisplayName = FText::FromString(Right);
						CategoryClass = Find;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("CL: Creating Category Failed...."));
						return nullptr;
					}
				}
			}
		}
	}
	else
	{
		CategoryClass = UBPFContentLib::FindClassWithLog(CategoryString, Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass(), Subsystem);
		if (!CategoryClass)
		{
			TSubclassOf<UFGItemCategory> Cat =  UBPFContentLib::CreateContentLibClass(CategoryString, Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass());
			if (Cat)
			{
				UE_LOG(LogTemp, Warning, TEXT("CL Recipes: Created Category %s "), *Cat->GetName())
				if(Schematic)
				{
					Subsystem->mSchematicCategories.Add(Cat);
					Cast<UFGSchematicCategory>(Cat->GetDefaultObject())->mDisplayName = FText::FromString(CategoryString);
				}
				else
				{
					Subsystem->mItemCategories.Add(Cat);
					Cast<UFGItemCategory>(Cat->GetDefaultObject())->mDisplayName = FText::FromString(CategoryString);
				}
				CategoryClass = Cat;
			}
			else
			{
				UClass* Find = FindObject<UClass>(ANY_PACKAGE, *CategoryString, false);
				if (!Find)
				{
					FString TempS = CategoryString;
					Find = FindObject<UClass>(ANY_PACKAGE, *TempS.Append("_C"), false);
				}
				if (Find && Find->IsChildOf(Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass()))
				{
					if(Schematic)
						Cast<UFGSchematicCategory>(Find->GetDefaultObject())->mDisplayName = FText::FromString(CategoryString);
					else
						Cast<UFGItemCategory>(Find->GetDefaultObject())->mDisplayName = FText::FromString(CategoryString);
					CategoryClass = Find;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("CL: Creating Category Failed."))
					return nullptr;
				}
			}
		}
	}
	return CategoryClass;
}


void UBPFContentLib::AddToItemAmountArray(TArray<FItemAmount> & Array,TMap<FString,int32> Cost,TArray<UClass*> Items, const bool ClearFirst)
{
	if (ClearFirst)
	{
		Array.Empty();
	}
	
	for(auto i :Cost)
	{
		if (i.Key.Contains("/"))
		{
			UClass* Loaded = LoadObject<UClass>(nullptr, *i.Key);
			if (Loaded && Loaded->IsChildOf(UFGItemDescriptor::StaticClass()))
			{
				FItemAmount Amount;
				Amount.Amount = i.Value;
				Amount.ItemClass = Loaded;
				bool Exists = false;
				for(auto & l : Array)
				{
					if(l.ItemClass == Loaded)
					{
						Exists = true;
						if(l.Amount != Amount.Amount)
						{
							l.Amount = Amount.Amount;
						}
						break;
					}
				}
				if(!Exists)
					Array.Add(Amount);
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Finding Item by Path %s failed"), *i.Key);
			}
		}
		else
		{
			bool Found = false;

			for(auto e : Items)
			{	
				TSubclassOf<class UFGItemDescriptor> Desc = e;
				if (UBPFContentLib::StringCompareItem(e->GetName(),i.Key,"Desc","_C"))
				{
					FItemAmount Amount;
					Amount.Amount = i.Value;
					Amount.ItemClass = Desc;
					bool Exists = false;
					for(auto & l : Array)
					{
						if(l.ItemClass == Desc)
						{
							Exists = true;
							if(l.Amount != Amount.Amount)
							{
								l.Amount = Amount.Amount;
							}
							break;
						}
					}
					if(!Exists)
						Array.Add(Amount);
					Found = true;
					break;
				}
			}

			if (!Found)
				UE_LOG(LogTemp, Error, TEXT("CL: Failed to find Item %s"), *i.Key)

		}
	}
}

void UBPFContentLib::AddRecipeToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const TSubclassOf<class UFGRecipe> Recipe)
{
	AFGSchematicManager * Manager = AFGSchematicManager::Get(Subsystem->GetWorld());
	AFGUnlockSubsystem * Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
	bool Added = false;
	for(auto f : Schematic.GetDefaultObject()->mUnlocks)
	{
		if(Cast<UFGUnlockRecipe>(f))
		{
			if (!Cast<UFGUnlockRecipe>(f)->mRecipes.Contains(Recipe))
			{
				Cast<UFGUnlockRecipe>(f)->mRecipes.Add(Recipe);
				Added = true;
				UE_LOG(LogTemp, Warning, TEXT("CL: Added Recipe to %s in Schematic %s "), *Recipe->GetName(), *Schematic->GetName())
				break;
			}
		}
	}
	if (!Added)
	{
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockRecipe_C"), false);
		if (!Class)
		{
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockRecipe.BP_UnlockRecipe_C"));
			if (!Class)
			{
				UE_LOG(LogTemp,Fatal,TEXT("CL: Couldnt find BP_UnlockRecipe_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockRecipe* Object = NewObject<UFGUnlockRecipe>(Schematic.GetDefaultObject(),Class);
		Object->mRecipes.Add(Recipe);
		Schematic.GetDefaultObject()->mUnlocks.Add(Object);
		UE_LOG(LogTemp, Warning, TEXT("CL: Created new Unlock. Added Recipe to %s in Schematic %s."), *Recipe->GetName(), *Schematic->GetName())

	}
	
}


void UBPFContentLib::AddSchematicToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const TSubclassOf<class UFGSchematic> SchematicToAdd)
{
	AFGSchematicManager * Manager = AFGSchematicManager::Get(Subsystem->GetWorld());
	AFGUnlockSubsystem * Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
	bool Added = false;
	for(auto f : Schematic.GetDefaultObject()->mUnlocks)
	{
		if(Cast<UFGUnlockSchematic>(f))
		{
			if (!Cast<UFGUnlockSchematic>(f)->mSchematics.Contains(SchematicToAdd))
			{
				Cast<UFGUnlockSchematic>(f)->mSchematics.Add(SchematicToAdd);
				Added = true;
				UE_LOG(LogTemp, Warning, TEXT("CL: Added Schematic to %s in Schematic %s "), *SchematicToAdd->GetName(), *Schematic->GetName())
				break;
			}
		}
	}
	if (!Added)
	{
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockSchematic_C"), false);
		if (!Class)
		{
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockSchematic.BP_UnlockSchematic_C"));
			if (!Class)
			{
				UE_LOG(LogTemp,Fatal,TEXT("CL: Couldnt find BP_UnlockSchematic_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockSchematic* Object = NewObject<UFGUnlockSchematic>(Schematic.GetDefaultObject(),Class);
		Object->mSchematics.Add(SchematicToAdd);
		Schematic.GetDefaultObject()->mUnlocks.Add(Object);
		UE_LOG(LogTemp, Warning, TEXT("CL: Created new Unlock. Added Recipe to %s in Schematic %s."), *SchematicToAdd->GetName(), *Schematic->GetName())

	}
	
}

void UBPFContentLib::AddSlotsToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const int32 Slots)
{
	AFGSchematicManager * Manager = AFGSchematicManager::Get(Subsystem->GetWorld());
	AFGUnlockSubsystem * Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
	bool Added = false;
	for(auto f : Schematic.GetDefaultObject()->mUnlocks)
	{
		if(Cast<UFGUnlockInventorySlot>(f))
		{
			if (!Cast<UFGUnlockInventorySlot>(f)->mNumInventorySlotsToUnlock != Slots)
			{
				Cast<UFGUnlockInventorySlot>(f)->mNumInventorySlotsToUnlock = Slots;
				Added = true;
				UE_LOG(LogTemp, Warning, TEXT("CL: Set UnlockSlots to %i in Schematic %s "), Slots, *Schematic->GetName())
				break;
			}
		}
	}
	if (!Added)
	{
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockInventorySlot_C"), false);
		if (!Class)
		{
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockInventorySlot.BP_UnlockInventorySlot_C"));
			if (!Class)
			{
				UE_LOG(LogTemp,Fatal,TEXT("CL: Couldnt find BP_UnlockSchematic_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockInventorySlot* Object = NewObject<UFGUnlockInventorySlot>(Schematic.GetDefaultObject(),Class);
		Object->mNumInventorySlotsToUnlock = Slots;
		Schematic.GetDefaultObject()->mUnlocks.Add(Object);
		UE_LOG(LogTemp, Warning, TEXT("CL: Created new Unlock. Set UnlockSlots to %i in Schematic %s "), Slots, *Schematic->GetName())
	}
	
}


void UBPFContentLib::AddGiveItemsToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const TMap<FString,int32> ItemsToGive, bool ClearFirst)
{
	for(auto f : Schematic.GetDefaultObject()->mUnlocks)
	{
		if(Cast<UFGUnlockGiveItem>(f))
		{
			AddToItemAmountArray(Cast<UFGUnlockGiveItem>(f)->mItemsToGive,ItemsToGive,Subsystem->mItems,ClearFirst);
			return;
		}
	}
	
	UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockGiveItem_C"), false);
	if (!Class)
	{
		Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockGiveItem.BP_UnlockGiveItem_C"));
		if (!Class)
		{
			UE_LOG(LogTemp,Fatal,TEXT("CL: Couldnt find BP_UnlockGiveItem wanting to Add to %s"), *Schematic->GetName())
		}
	}
	UFGUnlockGiveItem* Object = NewObject<UFGUnlockGiveItem>(Schematic.GetDefaultObject(),Class);
	AddToItemAmountArray(Object->mItemsToGive,ItemsToGive,Subsystem->mItems,ClearFirst);
	Schematic.GetDefaultObject()->mUnlocks.Add(Object);
	UE_LOG(LogTemp, Warning, TEXT("CL: Created new Unlock. Set for Items to Give in Schematic %s "), *Schematic->GetName())
}
void UBPFContentLib::AddArmSlotsToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const int32 Slots)
{
	bool Added = false;
	for(auto f : Schematic.GetDefaultObject()->mUnlocks)
	{
		if(Cast<UFGUnlockArmEquipmentSlot>(f))
		{
			if (!Cast<UFGUnlockArmEquipmentSlot>(f)->mNumArmEquipmentSlotsToUnlock != Slots)
			{
				Cast<UFGUnlockArmEquipmentSlot>(f)->mNumArmEquipmentSlotsToUnlock = Slots;
				Added = true;
				UE_LOG(LogTemp, Warning, TEXT("CL: Set Unlock Arm Slots to %i in Schematic %s "), Slots, *Schematic->GetName())
				break;
			}
		}
	}
	if (!Added)
	{
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockArmEquipmentSlot_C"), false);
		if (!Class)
		{
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockArmEquipmentSlot.BP_UnlockArmEquipmentSlot_C"));
			if (!Class)
			{
				UE_LOG(LogTemp,Fatal,TEXT("CL: Couldnt find BP_UnlockArmEquipmentSlot_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockArmEquipmentSlot* Object = NewObject<UFGUnlockArmEquipmentSlot>(Schematic.GetDefaultObject(),Class);
		Object->mNumArmEquipmentSlotsToUnlock = Slots;
		Schematic.GetDefaultObject()->mUnlocks.Add(Object);
		UE_LOG(LogTemp, Warning, TEXT("CL: Created new Unlock. Set Unlock Arm Slots to %i in Schematic %s "), Slots, *Schematic->GetName())
	}
	
}


void UBPFContentLib::UnlockUnlockedRecipes(UContentLibSubsystem* Subsystem)
{
	AFGUnlockSubsystem* Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
	AFGSchematicManager* Manager = AFGSchematicManager::Get(Subsystem->GetWorld());

	for (auto nRecipe : Subsystem->Recipes)
	{
		if (Manager && Unlock && Unlock->HasAuthority())
		{
			for (auto i : nRecipe.Value.nUnlockedBy)
			{
				if (Manager->IsSchematicPurchased(i))
				{
					Unlock->UnlockRecipe(nRecipe.Key);
					UE_LOG(LogTemp, Warning, TEXT("CL: Unlocked Recipe %s"), *nRecipe.Key->GetName())

				}
			}
		}
	}

	
}

void UBPFContentLib::AddSchematicToPurchaseDep(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, TSubclassOf<UFGSchematic> SchematicDep)
{
	bool Added = false;
	for(auto f : Schematic.GetDefaultObject()->mSchematicDependencies)
	{
		if(Cast<UFGSchematicPurchasedDependency>(f))
		{
			if (!Cast<UFGSchematicPurchasedDependency>(f)->mSchematics.Contains(SchematicDep))
			{
				Cast<UFGSchematicPurchasedDependency>(f)->mSchematics.Add(SchematicDep);
				Added = true;
				UE_LOG(LogTemp, Warning, TEXT("CL : Added SchematicDep to %s in Schematic %s "), *SchematicDep->GetName(), *Schematic->GetName())
				break;
			}
		}
	}
	if (!Added)
	{
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_SchematicPurchasedDependency_C"), false);
		if (!Class)
		{
			Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/AvailabilityDependencies/BP_SchematicPurchasedDependency.BP_SchematicPurchasedDependency_C"));
			if (!Class)
			{
				UE_LOG(LogTemp,Fatal,TEXT("CL: Couldnt find BP_SchematicPurchasedDependency_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGSchematicPurchasedDependency* Object = NewObject<UFGSchematicPurchasedDependency>(Schematic.GetDefaultObject(),Class);
		Object->mSchematics.Add(SchematicDep);
		Schematic.GetDefaultObject()->mSchematicDependencies.Add(Object);
		UE_LOG(LogTemp, Warning, TEXT("CL: Created new UFGSchematicPurchasedDependency. Added Schematic to %s in Schematic %s."), *SchematicDep->GetName(), *Schematic->GetName())

	}
}