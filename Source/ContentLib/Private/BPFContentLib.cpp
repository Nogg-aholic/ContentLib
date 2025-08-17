#include "BPFContentLib.h"

#include "Algo/Reverse.h"
#include "FGSchematic.h"
#include "FGSchematicCategory.h"
#include "FGSchematicManager.h"
#include "FGUnlockSubsystem.h"
#include "FGWorkBench.h"
#include "FGResearchTree.h"
#include "AvailabilityDependencies/FGSchematicPurchasedDependency.h"
#include "Resources/FGBuildingDescriptor.h"
#include "Resources/FGItemDescriptor.h"
#include "Unlocks/FGUnlockArmEquipmentSlot.h"
#include "Unlocks/FGUnlockGiveItem.h"
#include "Unlocks/FGUnlockInventorySlot.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "UObject/UObjectAllocator.h"
#include "Util/ImageLoadingUtil.h"
#include "Reflection/ClassGenerator.h"


TArray<FString> UBPFContentLib::GetBlueprintFunctionNames(UClass* BlueprintClass) {
	TArray<FString> Names;

	if (BlueprintClass) {
		for (auto i : BlueprintClass->*get(grab_FuncTable())) {
			Names.Add(i.Key.ToString());
		}
	}
	return Names;
}


UObject* UBPFContentLib::Conv_ClassToObject(UClass* Class) {
	if (Class) {
		return Class->GetDefaultObject();
	}
	return nullptr;
}

UTexture2D* UBPFContentLib::GetIconForBuilding(UContentLibSubsystem* System, TSubclassOf<AFGBuildable> Buildable, bool Big, const UObject* WorldContext)
{
	TArray<TSubclassOf<AFGBuildable>> Arr;
	if (System->BuildGunBuildings.Contains(Buildable)) {
		const TSubclassOf<class UFGBuildingDescriptor> Desc = *System->BuildGunBuildings.Find(Buildable);
		if (Big) {
			return Desc.GetDefaultObject()->GetBigIcon(Desc);
		}
		return Desc.GetDefaultObject()->GetSmallIcon(Desc);
	}
	return nullptr;
}


bool UBPFContentLib::SetStringIntMapFieldWithLog(TMap<FString, int32>& Field, FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return false;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::Array) {
		return false;
	}

	for (auto i : Result->TryGetField(FieldName)->AsArray()) {
		if (i->Type == EJson::Object) {
			const auto Obj = i->AsObject();
			const bool HasItem = Obj->HasField("Item");
			const bool HasAmount = Obj->HasField("Amount");
			if (HasItem && HasAmount) {
				const TSharedPtr<FJsonValue> Item = Obj->TryGetField("Item");
				const TSharedPtr<FJsonValue> Amount = Obj->TryGetField("Amount");

				if (Item->Type == EJson::String && Amount->Type == EJson::Number) {
					Field.Add(Item->AsString(), Amount->AsNumber());
				}
				else {
					if (Item->Type != EJson::String) {
						UE_LOG(LogContentLib, Error, TEXT("Invalid Type on Ingredient. Expected String"));
					}
					else {
						UE_LOG(LogContentLib, Error, TEXT("Invalid Type on Amount! Expected Number"));
					}
				}
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("No Item or Amount In Json"));
			}
		}
	}
	return true;
}


bool UBPFContentLib::SetColorFieldWithLog(FColor& Field, FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (Result->HasField(FieldName) && Result->TryGetField(FieldName)->Type == EJson::Object) {
		const auto Obj = Result->TryGetField(FieldName)->AsObject();
		if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a")) {
			if (Obj->TryGetField("r")->Type == EJson::Number) {
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.R, "r", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("R value in Color is not of type Number !"));
			}
			if (Obj->TryGetField("g")->Type == EJson::Number) {
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.G, "g", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("G value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("b")->Type == EJson::Number) {
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.B, "b", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("b value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("a")->Type == EJson::Number) {
				UBPFContentLib::SetSmallIntegerFieldWithLog(Field.A, "a", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("A value in Color is not of type Number !"));
			}
		}
		return true;
	}
	return false;
}


bool UBPFContentLib::SetLinearColorFieldWithLog(FLinearColor& Field, FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (Result->HasField(FieldName) && Result->TryGetField(FieldName)->Type == EJson::Object) {
		const auto Obj = Result->TryGetField(FieldName)->AsObject();
		if (Obj->HasField("r") && Obj->HasField("g") && Obj->HasField("b") && Obj->HasField("a")) {
			if (Obj->TryGetField("r")->Type == EJson::Number) {
				UBPFContentLib::SetFloatFieldWithLog(Field.R, "r", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("R value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("g")->Type == EJson::Number) {
				UBPFContentLib::SetFloatFieldWithLog(Field.G, "g", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("G value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("b")->Type == EJson::Number) {
				UBPFContentLib::SetFloatFieldWithLog(Field.B, "b", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("b value in Color is not of type Number !"));
			}

			if (Obj->TryGetField("a")->Type == EJson::Number) {
				UBPFContentLib::SetFloatFieldWithLog(Field.A, "a", Obj);
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("A value in Color is not of type Number !"));
			}
		}
		return true;
	}
	return false;
}

void UBPFContentLib::SetBooleanFieldWithLog(bool& Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::Boolean) {
		UE_LOG(LogContentLib, Error, TEXT("Field %s is not of Type Boolean"), *FieldName)
			return;
	}

	Field = Result->TryGetField(FieldName)->AsBool();
}

void UBPFContentLib::SetFloatFieldWithLog(float& Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::Number) {
		UE_LOG(LogContentLib, Error, TEXT("Field %s is not of Type Number"), *FieldName);
		return;
	}

	Field = Result->TryGetField(FieldName)->AsNumber();
}

void UBPFContentLib::SetIntegerFieldWithLog(int32& Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::Number && Result->TryGetField(FieldName)->Type != EJson::Boolean) {
		UE_LOG(LogContentLib, Error, TEXT("Field %s is not of Type Number or Boolean"), *FieldName);
		return;
	}

	if (Result->TryGetField(FieldName)->Type == EJson::Boolean) {
		Field = static_cast<int32>(Result->TryGetField(FieldName)->AsBool());
		return;
	}

	Field = Result->TryGetField(FieldName)->AsNumber();
}

void UBPFContentLib::SetSmallIntegerFieldWithLog(uint8& Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::Number && Result->TryGetField(FieldName)->Type != EJson::Boolean) {
		UE_LOG(LogContentLib, Error, TEXT("Field %s is not of Type Number or Boolean"), *FieldName);
		return;
	}

	if (Result->TryGetField(FieldName)->Type == EJson::Boolean) {
		Field = static_cast<uint8>(Result->TryGetField(FieldName)->AsBool());
		return;
	}

	Field = Result->TryGetField(FieldName)->AsNumber();
}

void UBPFContentLib::SetStringFieldWithLog(FString& Field, const FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::String) {
		UE_LOG(LogContentLib, Error, TEXT("Field %s is not of Type String"), *FieldName);
		return;
	}

	Field = Result->TryGetField(FieldName)->AsString();
}

bool UBPFContentLib::SetStringArrayFieldWithLog(TArray<FString>& Field, FString FieldName, TSharedPtr<FJsonObject> Result) {
	if (!Result->HasField(FieldName)) {
		return false;
	}

	if (Result->TryGetField(FieldName)->Type != EJson::Array) {
		UE_LOG(LogContentLib, Error, TEXT("Field %s is not of Type Array"), *FieldName);
		return false;
	}
	for (const auto& i : Result->TryGetField(FieldName)->AsArray()) {
		if (i->Type == EJson::String) {
			FString Item = i->AsString();
			if (Item != "") {
				if (!Field.Contains((Item))) {
					Field.Add(Item);
				}
			}
		}
		else {
			UE_LOG(LogContentLib, Error, TEXT("Field %s contains Elements that aren't Strings"), *FieldName)
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
	if (!AbsolutePath.Contains(TEXT(".."))) {
		FFileHelper::SaveStringToFile(resultString, *AbsolutePath);
	}
	else {
		UE_LOG(LogContentLib, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
	}
#endif
}

UTexture2D* UBPFContentLib::LoadTextureFromFile(FString& String, FString Path, bool Relative) {
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(Path);
	if (AbsolutePath.StartsWith(AbsoluteRootPath)) {
		FString OutErrorMessage;
		UTexture2D* LoadedModIcon = FImageLoadingUtil::LoadImageFromFile(*AbsolutePath, OutErrorMessage);
		return LoadedModIcon;
	}

	UE_LOG(LogContentLib, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
	return nullptr;

}

bool UBPFContentLib::LoadStringFromFile(FString& String, FString Path, bool Relative) {
#if WITH_EDITOR
	return FFileHelper::LoadFileToString(String, Relative ? *(FPaths::ProjectDir() + Path) : *Path);
#else
	const FString AbsoluteRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(Path);
	if (AbsolutePath.StartsWith(AbsoluteRootPath)) {
		return FFileHelper::LoadFileToString(String, *AbsolutePath);
	}

	UE_LOG(LogContentLib, Error, TEXT("Absolute or escaping Paths are not allowed in Runtime"));
	return false;
#endif
}

bool UBPFContentLib::GetDirectoriesInPath(const FString& FullPathOfBaseDir, TArray<FString>& DirsOut, const FString& NotContainsStr, bool Recursive, const FString& ContainsStr) {
	FString Str;
	auto FilenamesVisitor = MakeDirectoryVisitor([&](const TCHAR* FilenameOrDirectory, bool bIsDirectory) {
		if (bIsDirectory) {
			//Using a Contains Filter?
			if (ContainsStr != "") {
				Str = FPaths::GetCleanFilename(FilenameOrDirectory);
				//Only if Directory Contains Str
				if (Str.Contains(ContainsStr)) {
					if (Recursive) {
						DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
					}
					else {
						DirsOut.Push(Str);
					}
				}

			}
			else if (NotContainsStr != "") {
				if (!Str.Contains(NotContainsStr)) {
					if (Recursive) {
						DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
					}
					else {
						DirsOut.Push(Str);
					}
				}
			}
			//Get ALL Directories!
			else {
				//Just the Directory
				Str = FPaths::GetCleanFilename(FilenameOrDirectory);
				if (Recursive) {
					DirsOut.Push(FilenameOrDirectory); //need whole path for recursive
				}
				else {
					DirsOut.Push(Str);
				}
			}
		}
		return true;
		}
	);

	if (Recursive) {
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FullPathOfBaseDir, FilenamesVisitor);
	}
	return FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPathOfBaseDir, FilenamesVisitor);
}

void UBPFContentLib::String_Sort(UPARAM(ref) TArray <FString>& Array_To_Sort, bool Descending, bool FilterToUnique, TArray <FString>& Sorted_Array)
{
	if (FilterToUnique) {
		for (auto i : Array_To_Sort) {
			if (FilterToUnique) {
				if (!Sorted_Array.Contains(i)) {
					Sorted_Array.Add(i);
				}
			}
		}
	}
	Sorted_Array.Sort();               // Sort array using built in function (sorts A-Z)

	if (Descending == true) {
		Algo::Reverse(Sorted_Array);
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
			if (!bIsDirectory) {
				Str = FPaths::GetCleanFilename(FilenameOrDirectory);

				//Filter by Extension if specified, otherwise Include All Filenames!
				if (FileExt == "" || FPaths::GetExtension(Str).ToLower() == FileExt) {
					if (Recursive) {
						FilenamesOut.Push(FilenameOrDirectory); //need whole path for recursive
					} else {
						FilenamesOut.Push(Str);
					}
				}
			}
			return true;
		}
	);

	if (Recursive) {
		return FPlatformFileManager::Get().GetPlatformFile().IterateDirectoryRecursively(*FullPathOfBaseDir, FilenamesVisitor);
	}

	return FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*FullPathOfBaseDir, FilenamesVisitor);
}


TSubclassOf<UObject> UBPFContentLib::FindClassWithLog(FString Name, UClass* ParentClass, UContentLibSubsystem* RootFolder)
{
	// UE_LOG(LogContentLib, Warning, TEXT("FindClassWithLog %s by Name %s"), *ParentClass->GetName(), *Name);
	if (!ParentClass) {
		UE_LOG(LogContentLib, Error, TEXT("FindClassWithLog %s by Name failed. Empty Parent Class !"), *Name);
		return nullptr;
	}

	if (Name == "") {
		UE_LOG(LogContentLib, Error, TEXT("FindClassWithLog failed. Empty Name! Parent class was %s"), *ParentClass->GetName());
		return nullptr;
	}
	if (Name.Contains("/")) {
		// UE_LOG(LogContentLib, Error, TEXT("FindClassWithLog has slash so using LoadObject on %s"), *Name);
		UClass* Loaded = LoadObject<UClass>(nullptr, *Name);
		if (Loaded && Loaded->IsChildOf(ParentClass))
			return Loaded;
	}
	else {
		if (Name.Contains(" ")) {
			Name = Name.Replace(TEXT(" "), TEXT("-"), ESearchCase::CaseSensitive);
		}
		FString Prefix;
		FString Suffix = "_C";
		TArray<UClass*> ClassArr;

		if (ParentClass->IsChildOf(UFGItemDescriptor::StaticClass())) {
			ClassArr = RootFolder->mItems;
			TArray<TSubclassOf<UFGItemDescriptor>> Arr;
			RootFolder->CreatedItems.GetKeys(Arr);
			ClassArr.Append(Arr);
			Prefix = "Desc_";
		}
		else if (ParentClass->IsChildOf(UFGCategory::StaticClass())) {
			ClassArr = RootFolder->mCategories;
			Prefix = "Cat_";
		}
		else if (ParentClass->IsChildOf(UFGSchematic::StaticClass())) {
			ClassArr = RootFolder->mSchematics;
			TArray<TSubclassOf<UFGSchematic>> Arr;
			RootFolder->CreatedSchematics.GetKeys(Arr);
			ClassArr.Append(Arr);
			Prefix = "";
		}
		else if (ParentClass->IsChildOf(UFGWorkBench::StaticClass())) {
			ClassArr = RootFolder->mCraftingComps;
			Prefix = "";
		}
		else if (ParentClass->IsChildOf(UFGRecipe::StaticClass())) {
			ClassArr = RootFolder->mRecipes;
			TArray<TSubclassOf<UFGRecipe>> Arr;
			RootFolder->CreatedRecipes.GetKeys(Arr);
			ClassArr.Append(Arr);
			Prefix = "Recipe_";
		}
		else if (ParentClass->IsChildOf(UFGResearchTree::StaticClass())) {
			ClassArr = RootFolder->mResearchTrees;
			Prefix = "";
		}
		else if (ParentClass->IsChildOf(UObject::StaticClass())) {
			ClassArr = RootFolder->mBuilders;

			Prefix = "Build_";
		}
		else {
			Prefix = "";
		}

		// UE_LOG(LogContentLib, Warning, TEXT("There are %d things to seatch for in ClassArr, name is %s"), ClassArr.Num(), *Name);

		for (auto candidate : ClassArr) {
			FString candidateName = candidate->GetName();
			FString targetNameWithSuffix = Name;
			targetNameWithSuffix.Append(Suffix);
			FString targetNameWithPrefixAndSuffix = FString(Prefix).Append(Name);
			if (!Name.EndsWith(Suffix)) { // Tack on suffix if it's not already there
				targetNameWithPrefixAndSuffix.Append(Suffix);
			}
			// UE_LOG(LogContentLib, Warning, TEXT("Searching %s against names: %s | %s | %s"), *objectName, *Name, *targetNameWithSuffix, *targetNameWithPrefixAndSuffix);

			if (candidateName.Equals(Name, ESearchCase::IgnoreCase) || // Name exactly
				candidateName.Equals(targetNameWithSuffix, ESearchCase::IgnoreCase) || // Name and Just Suffix
				candidateName.Equals(targetNameWithPrefixAndSuffix, ESearchCase::IgnoreCase)) // Prefix and Name and Suffix
			{
				// UE_LOG(LogContentLib, Warning, TEXT("Matched candidate '%s' against query names: %s | %s | %s"), *candidateName, *Name, *targetNameWithSuffix, *targetNameWithPrefixAndSuffix);
				return candidate;
			}
		}
	}
	UE_LOG(LogContentLib, Warning, TEXT("Finding %s by Name %s in CL's records failed, not necessarily an error"), *ParentClass->GetName(), *Name);
	return nullptr;
}

TSubclassOf<UObject> UBPFContentLib::CreateContentLibClass(FString Name, UClass* ParentClass)
{
	if (Name == "" || FindObject<UClass>(ANY_PACKAGE, *Name, false) || FindObject<UClass>(ANY_PACKAGE, *Name.Append("_C"), false)) {
		if (Name == "")
			UE_LOG(LogContentLib, Error, TEXT("Name was empty, can't create class"));
		return nullptr;
	}

	return FClassGenerator::GenerateSimpleClass(TEXT("/ContentLib/"), *Name, ParentClass);
}


bool UBPFContentLib::StringCompareItem(FString toCompare, FString Name, FString Prefix, FString Suffix)
{
	FString PrefixNameSuffix = Prefix.Append(Name);
	if (!PrefixNameSuffix.EndsWith(Suffix)) {
		PrefixNameSuffix.Append(Suffix);
	}
	return
		toCompare.Equals(Name, ESearchCase::IgnoreCase) ||
		toCompare.Equals(Name.Append(Suffix), ESearchCase::IgnoreCase) ||
		toCompare.Equals(PrefixNameSuffix, ESearchCase::IgnoreCase);
}

UClass* UBPFContentLib::SetCategoryWithLoad(FString CategoryString, UContentLibSubsystem* Subsystem, bool Schematic)
{
	UClass* CategoryClass = nullptr;
	UClass* CategoryParentClass = Schematic ? UFGSchematicCategory::StaticClass() : UFGItemCategory::StaticClass();
	bool needToCreateClass = false;
	FString CategoryNameToCreate = "Cat_";

	if (CategoryString.Contains(" ")) {
		CategoryString = CategoryString.Replace(TEXT(" "), TEXT("-"), ESearchCase::CaseSensitive);
	}

	if (CategoryString.Contains("/")) {
		// Interpret as full path

		UClass* Loaded = LoadObject<UClass>(nullptr, *CategoryString);
		if (Loaded && Loaded->IsChildOf(UFGCategory::StaticClass())) {
			// found it, use it
			return Loaded;
		}
		else {
			// need to create it since we couldn't find it
			FString Left;
			FString RightOfLastSlash;
			CategoryString.Split("/", &Left, &RightOfLastSlash, ESearchCase::CaseSensitive, ESearchDir::FromEnd);

			if (RightOfLastSlash.IsEmpty()) {
				UE_LOG(LogContentLib, Error, TEXT("CL: Right side of full path is empty, probably broken path %s"), *CategoryString);
			}
			else {
				needToCreateClass = true;
				CategoryNameToCreate.Append(RightOfLastSlash);
			}
		}
	}
	else {
		// Look up by name since not a full path

		// FindClassWithLog already handles checking with the Cat_ prefix
		CategoryClass = UBPFContentLib::FindClassWithLog(CategoryString, UFGCategory::StaticClass(), Subsystem);
		if (CategoryClass) {
			// UE_LOG(LogContentLib, Warning, TEXT("Successfully found class %s in CL records"), *CategoryString);
			return CategoryClass;
		}
		else {
			needToCreateClass = true;
			CategoryNameToCreate.Append(CategoryString); // TODO see if this mutates (pretty sure it does?)
			// UE_LOG(LogContentLib, Warning, TEXT("Could not find find class %s in CL records, need to create it, will be asset name %s"), *CategoryString, *CategoryNameToCreate);
		}
	}

	if (needToCreateClass) {
		// Couldn't find in CL's own records, so create it

		const TSubclassOf<UObject> CreatedCategory = UBPFContentLib::CreateContentLibClass(CategoryNameToCreate, CategoryParentClass);
		if (CreatedCategory) {
			UE_LOG(LogContentLib, Warning, TEXT("CL: Created Category with name %s"), *CreatedCategory->GetName())
				Cast<UFGCategory>(CreatedCategory->GetDefaultObject())->mDisplayName = FText::FromString(CategoryString);
			Subsystem->mCategories.Add(CreatedCategory);
			CategoryClass = CreatedCategory;
		}
		else {
			// Creation failed, so try more rigorous lookup for existing conflicting asset? Left over from Nog code, not sure if this is wise
			// UE_LOG(LogContentLib, Warning, TEXT("CL: Rigor lookup for %s"), *CategoryNameToCreate)
			UClass* FoundClass = FindObject<UClass>(ANY_PACKAGE, *CategoryNameToCreate, false);
			if (!FoundClass) {
				// Try finding with _C suffix
				// UE_LOG(LogContentLib, Warning, TEXT("CL: Rigor lookup for %s with _C"), *CategoryNameToCreate)
				FoundClass = FindObject<UClass>(ANY_PACKAGE, *CategoryNameToCreate.Append("_C"), false);
			}

			if (FoundClass && FoundClass->IsChildOf(UFGCategory::StaticClass())) {
				// Found something else; coerce its display name into what we wanted
				Cast<UFGCategory>(FoundClass->GetDefaultObject())->mDisplayName = FText::FromString(CategoryString);
				UE_LOG(LogContentLib, Warning, TEXT("CL: Coerced existing Category by name %s into ours"), *CategoryString);
				CategoryClass = FoundClass;
			}
			else {
				UE_LOG(LogContentLib, Error, TEXT("CL: Creating Category (FullPath) Failed. %s "), *CategoryString);
				return nullptr;
			}
		}
	}
	return CategoryClass;
}

bool UBPFContentLib::ContainsInvalidItem(TMap<FString, int32> Cost, TArray<UClass*> AllKnownItems)
{
	for (auto& entry : Cost) {
		if (entry.Key.Contains("/")) {
			UClass* Loaded = LoadObject<UClass>(nullptr, *entry.Key);
			if (!(Loaded && Loaded->IsChildOf(UFGItemDescriptor::StaticClass()))) {
				UE_LOG(LogContentLib, Error, TEXT("Finding Item by Path %s failed"), *entry.Key);
				return true;
			}
		} else {
			// TODO kinda expensive search, optimization potential
			bool found = false;
			for (auto possibleItemMatch : AllKnownItems) {
				if (UBPFContentLib::StringCompareItem(possibleItemMatch->GetName(), entry.Key, "Desc", "_C")) {
					// UE_LOG(LogContentLib, VeryVerbose, TEXT("CL DEBUG: Found item %s as: %s"), *entry.Key, *possibleItemMatch->GetName());
					found = true;
					break;
				}
			}
			if (!found) {
				UE_LOG(LogContentLib, Error, TEXT("Failed to find Item '%s'"), *entry.Key);
				return true;
			}
		}
	}
	return false;
}

void UBPFContentLib::AddToItemAmountArray(TArray<FItemAmount>& Array, TMap<FString, int32> Cost, TArray<UClass*> Items, const bool ClearFirst)
{
	if (ClearFirst) {
		Array.Empty();
	}
	for (auto& entry : Cost) {
		if (entry.Key.Contains("/")) {
			UClass* Loaded = LoadObject<UClass>(nullptr, *entry.Key);
			if (Loaded && Loaded->IsChildOf(UFGItemDescriptor::StaticClass())) {
				FItemAmount Amount;
				Amount.Amount = entry.Value;
				Amount.ItemClass = Loaded;
				bool Exists = false;
				for (auto& l : Array) {
					if (l.ItemClass == Loaded) {
						Exists = true;
						if (l.Amount != Amount.Amount) {
							l.Amount = Amount.Amount;
						}
						break;
					}
				}
				if (!Exists)
					Array.Add(Amount);
			} else {
				UE_LOG(LogContentLib, Error, TEXT("Finding Item by Path %s failed"), *entry.Key);
			}
		}
		else
		{
			bool Found = false;
			// TODO kinda expensive search, optimization potential
			for (auto possibleItemMatch : Items) {
				TSubclassOf<class UFGItemDescriptor> Desc = possibleItemMatch;
				if (UBPFContentLib::StringCompareItem(possibleItemMatch->GetName(), entry.Key, "Desc", "_C")) {
					FItemAmount Amount;
					Amount.Amount = entry.Value;
					Amount.ItemClass = Desc;

					// Consolidate duplicate cost entries in the final output array
					bool Exists = false;
					for (auto& outputArrayEntry : Array) {
						if (outputArrayEntry.ItemClass == Desc) {
							Exists = true;
							if (outputArrayEntry.Amount != Amount.Amount) {
								outputArrayEntry.Amount = Amount.Amount;
							}
							break;
						}
					}
					if (!Exists)
						Array.Add(Amount);

					Found = true;
					break;
				}
			}
			if (!Found)
				UE_LOG(LogContentLib, Error, TEXT("CL: Failed to find Item %s"), *entry.Key)
		}
	}
}

void UBPFContentLib::AddRecipeToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, const TSubclassOf<class UFGRecipe> Recipe)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	bool Added = false;
	for (auto f : CDO->mUnlocks) {
		if (UFGUnlockRecipe* unlock = Cast<UFGUnlockRecipe>(f)) {
			if (!unlock->mRecipes.Contains(Recipe)) {
				unlock->mRecipes.Add(Recipe);
				Added = true;
				UE_LOG(LogContentLib, Warning, TEXT("CL: Added Recipe %s to unlocks of Schematic %s."), *Recipe->GetName(), *Schematic->GetName())
					break;
			}
		}
	}
	if (!Added) {
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockRecipe_C"), false);
		if (!Class) {
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockRecipe.BP_UnlockRecipe_C"));
			if (!Class) {
				UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't find BP_UnlockRecipe_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockRecipe* Object = NewObject<UFGUnlockRecipe>(CDO, Class);
		Object->mRecipes.Add(Recipe);
		CDO->mUnlocks.Add(Object);
		UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Unlock. Added Recipe %s to unlocks of Schematic %s."), *Recipe->GetName(), *Schematic->GetName())
	}
}

void UBPFContentLib::AddSchematicToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, const TSubclassOf<class UFGSchematic> SchematicToAdd)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	bool Added = false;
	for (auto f : CDO->mUnlocks) {
		if (UFGUnlockSchematic* unlock = Cast<UFGUnlockSchematic>(f)) {
			if (!unlock->mSchematics.Contains(SchematicToAdd)) {
				unlock->mSchematics.Add(SchematicToAdd);
				Added = true;
				UE_LOG(LogContentLib, Warning, TEXT("CL: Added Schematic %s to unlocks of Schematic %s."), *SchematicToAdd->GetName(), *Schematic->GetName())
					break;
			}
		}
	}
	if (!Added) {
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockSchematic_C"), false);
		if (!Class) {
			Class = LoadObject<UClass>(FindPackage(nullptr, TEXT("/Game/")), TEXT("/Game/FactoryGame/Unlocks/BP_UnlockSchematic.BP_UnlockSchematic_C"));
			if (!Class) {
				UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't find BP_UnlockSchematic_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockSchematic* Object = NewObject<UFGUnlockSchematic>(CDO, Class);
		Object->mSchematics.Add(SchematicToAdd);
		CDO->mUnlocks.Add(Object);
		UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Unlock. Added Schematic %s to unlocks of Schematic %s."), *SchematicToAdd->GetName(), *Schematic->GetName())
	}
}

void UBPFContentLib::AddInfoOnlyToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, FContentLib_UnlockInfoOnly InfoCardToAdd)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	UClass* Class = FindObject<UClass>(FindPackage(nullptr, TEXT("/Game/")), TEXT("BP_UnlockInfoOnly_C"), false);
	if (!Class) {
		Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockInfoOnly.BP_UnlockInfoOnly_C"));
		if (!Class) {
			UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't find BP_UnlockInfoOnly_C wanting to Add to %s"), *Schematic->GetName())
		}
	}
	UFGUnlockInfoOnly* NewEntry = NewObject<UFGUnlockInfoOnly>(CDO, Class);
	NewEntry->mUnlockName = InfoCardToAdd.mUnlockName;
	NewEntry->mUnlockDescription = InfoCardToAdd.mUnlockDescription;

	// TODO consider switching to subsystem find
	//auto big = Subsystem->Icons.Find(InfoCardToAdd.BigIcon);
	auto big = LoadObject<UTexture2D>(nullptr, *InfoCardToAdd.BigIcon);
	if (!big) {
		UE_LOG(LogContentLib, Warning, TEXT("Failed to find BigIcon %s"), *InfoCardToAdd.BigIcon);
	} else {
		UE_LOG(LogContentLib, Warning, TEXT("Found BigIcon %s"), *InfoCardToAdd.BigIcon);
		NewEntry->mUnlockIconBig = big;
	}

	auto smallIcon = LoadObject<UTexture2D>(nullptr, *InfoCardToAdd.SmallIcon);
	if (!smallIcon) {
		UE_LOG(LogContentLib, Warning, TEXT("Failed to find SmallIcon %s"), *InfoCardToAdd.SmallIcon);
	} else {
		UE_LOG(LogContentLib, Warning, TEXT("Found SmallIcon %s"), *InfoCardToAdd.SmallIcon);
		NewEntry->mUnlockIconBig = smallIcon;
	}

	auto category = LoadObject<UTexture2D>(nullptr, *InfoCardToAdd.CategoryIcon);
	if (!category) {
		UE_LOG(LogContentLib, Warning, TEXT("Failed to find BigIcon %s"), *InfoCardToAdd.CategoryIcon);
	} else {
		UE_LOG(LogContentLib, Warning, TEXT("Found BigIcon %s"), *InfoCardToAdd.CategoryIcon);
		NewEntry->mUnlockIconCategory = category;
	}

	CDO->mUnlocks.Add(NewEntry);
	UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Unlock. Added InfoCard %s in Schematic %s."), *InfoCardToAdd.mUnlockName.ToString(), *Schematic->GetName())
}

void UBPFContentLib::AddInventorySlotsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, const int32 Slots)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	bool Added = false;
	for (auto f : CDO->mUnlocks) {
		if (const auto unlock = Cast<UFGUnlockInventorySlot>(f)) {
			if (!(unlock->mNumInventorySlotsToUnlock != Slots)) {
				unlock->mNumInventorySlotsToUnlock = Slots;
				Added = true;
				UE_LOG(LogContentLib, Warning, TEXT("CL: Set UnlockSlots to %i in Schematic %s "), Slots, *Schematic->GetName())
					break;
			}
		}
	}
	if (!Added) {
		UClass* Class = FindObject<UClass>(FindPackage(nullptr, TEXT("/Game/")), TEXT("BP_UnlockInventorySlot_C"), false);
		if (!Class) {
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockInventorySlot.BP_UnlockInventorySlot_C"));
			if (!Class) {
				UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't find BP_UnlockSchematic_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockInventorySlot* Object = NewObject<UFGUnlockInventorySlot>(CDO, Class);
		Object->mNumInventorySlotsToUnlock = Slots;
		CDO->mUnlocks.Add(Object);
		UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Unlock. Set UnlockSlots to %i in Schematic %s "), Slots, *Schematic->GetName())
	}
}

void UBPFContentLib::AddGiveItemsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, const TMap<FString, int32> ItemsToGive, bool ClearFirst)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	for (auto f : CDO->mUnlocks) {
		if (const auto unlock = Cast<UFGUnlockGiveItem>(f)) {
			AddToItemAmountArray(unlock->mItemsToGive, ItemsToGive, Subsystem->mItems, ClearFirst);
			return;
		}
	}

	UClass* Class = FindObject<UClass>(FindPackage(nullptr, TEXT("/Game/")), TEXT("BP_UnlockGiveItem_C"), false);
	if (!Class) {
		Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockGiveItem.BP_UnlockGiveItem_C"));
		if (!Class) {
			UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't find BP_UnlockGiveItem wanting to Add to %s"), *Schematic->GetName())
		}
	}
	UFGUnlockGiveItem* Object = NewObject<UFGUnlockGiveItem>(CDO, Class);
	AddToItemAmountArray(Object->mItemsToGive, ItemsToGive, Subsystem->mItems, ClearFirst);
	CDO->mUnlocks.Add(Object);
	UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Unlock. Set for Items to Give in Schematic %s "), *Schematic->GetName())
}

void UBPFContentLib::AddArmSlotsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, const int32 Slots)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	bool Added = false;
	for (auto f : CDO->mUnlocks) {
		if (const auto unlock = Cast<UFGUnlockArmEquipmentSlot>(f)) {
			if (!(unlock->mNumArmEquipmentSlotsToUnlock != Slots)) {
				unlock->mNumArmEquipmentSlotsToUnlock = Slots;
				Added = true;
				UE_LOG(LogContentLib, Warning, TEXT("CL: Set Unlock Arm Slots to %i in Schematic %s "), Slots, *Schematic->GetName())
					break;
			}
		}
	}
	if (!Added) {
		UClass* Class = FindObject<UClass>(FindPackage(nullptr, TEXT("/Game/")), TEXT("BP_UnlockArmEquipmentSlot_C"), false);
		if (!Class) {
			Class = LoadObject<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockArmEquipmentSlot.BP_UnlockArmEquipmentSlot_C"));
			if (!Class) {
				UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't find BP_UnlockArmEquipmentSlot_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockArmEquipmentSlot* Object = NewObject<UFGUnlockArmEquipmentSlot>(CDO, Class);
		Object->mNumArmEquipmentSlotsToUnlock = Slots;
		CDO->mUnlocks.Add(Object);
		UE_LOG(LogContentLib, Warning, TEXT("CL: Created new Unlock. Set Unlock Arm Slots to %i in Schematic %s "), Slots, *Schematic->GetName())
	}
}


void UBPFContentLib::UnlockUnlockedRecipes(UContentLibSubsystem* Subsystem)
{
	AFGUnlockSubsystem* Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
	AFGSchematicManager* Manager = AFGSchematicManager::Get(Subsystem->GetWorld());

	if (!IsValid(Manager) || !IsValid(Unlock) || !Unlock->HasAuthority()) {
		return;
	}

	for (auto nRecipe : Subsystem->Recipes) {
		for (auto i : nRecipe.Value.nUnlockedBy) {
			if (Manager->IsSchematicPurchased(i)) {
				Unlock->UnlockRecipe(nRecipe.Key);
				UE_LOG(LogContentLib, Warning, TEXT("CL: Unlocked Recipe %s"), *nRecipe.Key->GetName())
			}
		}
	}
}

UFGSchematicPurchasedDependency* UBPFContentLib::FindFirstOrCreateSchematicPurchasedDependencyObj(TSubclassOf<UFGSchematic> Schematic)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		UE_LOG(LogContentLib, Fatal, TEXT("CL: Invalid schematic passed to %s"), *FString(__func__));
		return nullptr;
	}

	for (auto depObject : CDO->mSchematicDependencies) {
		if (const auto purchasedDepObject = Cast<UFGSchematicPurchasedDependency>(depObject)) {
			return purchasedDepObject;
		}
	}
	// at this point we haven't found one, so we need to make one
	UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_SchematicPurchasedDependency_C"), false);
	if (!Class) {
		Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/AvailabilityDependencies/BP_SchematicPurchasedDependency.BP_SchematicPurchasedDependency_C"));
		if (!Class) {
			UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't load BP_SchematicPurchasedDependency_C wanting to Add to %s"), *Schematic->GetName());
		}
	}
	return NewObject<UFGSchematicPurchasedDependency>(CDO, Class);
}

void UBPFContentLib::AddSchematicToPurchaseDep(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, TSubclassOf<UFGSchematic> SchematicDep)
{
	const auto CDO = Schematic.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	for (auto depObject : CDO->mSchematicDependencies) {
		if (const auto purchasedDepObject = Cast<UFGSchematicPurchasedDependency>(depObject)) {
			if (!purchasedDepObject->mSchematics.Contains(SchematicDep)) {
				purchasedDepObject->mSchematics.Add(SchematicDep);
				UE_LOG(LogContentLib, Warning, TEXT("CL : Added SchematicDep entry to existing: '%s' as a requirement for Schematic '%s' "), *SchematicDep->GetName(), *Schematic->GetName());
				return;
			} else {
				UE_LOG(LogContentLib, Warning, TEXT("CL : Skipped adding SchematicDep entry: '%s' is already a requirement for Schematic '%s' "), *SchematicDep->GetName(), *Schematic->GetName())
				return;
			}
		}
	}
	// No existing
	UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_SchematicPurchasedDependency_C"), false);
	if (!Class) {
		Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/AvailabilityDependencies/BP_SchematicPurchasedDependency.BP_SchematicPurchasedDependency_C"));
		if (!Class) {
			UE_LOG(LogContentLib, Fatal, TEXT("CL: Couldn't load BP_SchematicPurchasedDependency_C wanting to Add to %s"), *Schematic->GetName());
		}
	}
	UFGSchematicPurchasedDependency* purchasedDepObject = NewObject<UFGSchematicPurchasedDependency>(CDO, Class);
	purchasedDepObject->mSchematics.Add(SchematicDep);
	CDO->mSchematicDependencies.Add(purchasedDepObject);
	UE_LOG(LogContentLib, Warning, TEXT("CL: Created new UFGSchematicPurchasedDependency and added entry: '%s' as a requirement for Schematic '%s'"), *SchematicDep->GetName(), *Schematic->GetName());
}

bool UBPFContentLib::FailsBasicJsonFormCheck(FString jsonString)
{
	if (jsonString.IsEmpty() || !jsonString.StartsWith("{") || !jsonString.EndsWith("}")) {
		if (jsonString.IsEmpty()) {
			UE_LOG(LogContentLib, Error, TEXT("Invalid json - Empty String  %s"), *jsonString);
		} else if (!jsonString.StartsWith("{")) {
			UE_LOG(LogContentLib, Error, TEXT("Invalid json - String doesnt start with '{': %s"), *jsonString);
		} else if (!jsonString.EndsWith("}")) {
			UE_LOG(LogContentLib, Error, TEXT("Invalid json - String doesnt end with '}':  %s"), *jsonString);
		}
		return true;
	}
	return false;
}

bool UBPFContentLib::CL_ExecuteArbitraryFunction(FName functionName, UObject* onObject) {
	if (!onObject) {
		UE_LOG(LogContentLib, Error, TEXT("CL_ExecuteArbitraryFunction Passed onObject is invalid"));
		return false;
	}
	if (functionName.IsNone()) {
		UE_LOG(LogContentLib, Error, TEXT("CL_ExecuteArbitraryFunction Passed functionName is invalid"));
		return false;
	}
	UE_LOG(LogContentLib, Warning, TEXT("CL_ExecuteArbitraryFunction calling function '%s' on object '%s'"), *functionName.ToString(), *UKismetSystemLibrary::GetDisplayName(onObject));
	auto function = onObject->GetClass()->FindFunctionByName(functionName);
	auto valid = function->IsValidLowLevelFast();
	if (valid) {
		onObject->ProcessEvent(function, (void*) nullptr);
	}
	return valid;
}

FString UBPFContentLib::CL_GetExecutingPackage() {
	UE_LOG(LogContentLib, Error, TEXT("CL_GetExecutingPackage underlying C++ function called, this function is blueprint only"));
	return FString(TEXT("(C++ call stack)"));
}
