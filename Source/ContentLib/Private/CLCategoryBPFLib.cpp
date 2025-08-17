


#include "CLCategoryBPFLib.h"
#include "ContentLib.h"

#include "BPFContentLib.h"
#include "FGBuildCategory.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

// TODO do we still need variants instead of just using UFGCategory?

FString UCLCategoryBPFLib::GenerateFromItemCategory(TSubclassOf<UFGItemCategory> Item)
{
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return "";
	}

	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto MenuPriority = MakeShared<FJsonValueNumber>(CDO->mMenuPriority);
	const auto Obj = MakeShared<FJsonObject>();

	Obj->Values.Add("Name", Name);
	Obj->Values.Add("MenuPriority", MenuPriority);

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FContentLib_ItemCategory UCLCategoryBPFLib::CLItemCategoryFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_ItemCategory();
	}
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid()) {
		UE_LOG(LogContentLib, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_ItemCategory();
	}

	FContentLib_ItemCategory Cat;

	UBPFContentLib::SetStringFieldWithLog(Cat.Name, "Name", Result);
	UBPFContentLib::SetFloatFieldWithLog(Cat.MenuPriority, "MenuPriority", Result);

	return Cat;
}

FString UCLCategoryBPFLib::GenerateFromSchematicCategory(TSubclassOf<UFGSchematicCategory> Item)
{
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return "";
	}

	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto Icon = MakeShared<FJsonValueString>(CDO->mCategoryIcon.GetResourceObject() ? CDO->mCategoryIcon.GetResourceObject()->GetPathName() : "");

	const auto Obj = MakeShared<FJsonObject>();

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("Icon", Icon);

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FContentLib_SchematicCategory UCLCategoryBPFLib::CLSchematicCategoryFromString(FString String,UContentLibSubsystem* Subsystem)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_SchematicCategory();
	}
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid()) {
		UE_LOG(LogContentLib, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_SchematicCategory();
	}

	FContentLib_SchematicCategory Cat;

	UBPFContentLib::SetStringFieldWithLog(Cat.Name, "Name", Result);
	UBPFContentLib::SetStringFieldWithLog(Cat.Icon, "Icon", Result);

	return Cat;
}

void UCLCategoryBPFLib::InitItemCategoryFromStruct(FContentLib_ItemCategory SchematicCategory,TSubclassOf<UFGItemCategory> SchematicCategoryClass,UContentLibSubsystem *Subsystem)
{
	UFGItemCategory * CDO = SchematicCategoryClass.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}
	
	if (SchematicCategory.Name != "") {
		CDO->mDisplayName = FText::FromString(SchematicCategory.Name);
	}

	if (SchematicCategory.MenuPriority != -1) {
		CDO->mMenuPriority = SchematicCategory.MenuPriority;
	}
}

void UCLCategoryBPFLib::InitCategoryFromSchematicStruct(FContentLib_SchematicCategory SchematicCategory, TSubclassOf<UFGCategory> CategoryClass, UContentLibSubsystem* Subsystem)
{
	UFGCategory* CDO = CategoryClass.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}

	if (SchematicCategory.Name != "") {
		CDO->mDisplayName = FText::FromString(SchematicCategory.Name);
	}

	if (SchematicCategory.Icon != "" && IsValid(Subsystem)) {
		if (Subsystem->ImportedVisualKits.Contains(SchematicCategory.Icon)) {
			ApplyVisualKitToCategory(Subsystem, *Subsystem->ImportedVisualKits.Find(SchematicCategory.Icon), CategoryClass);
		} else if (Subsystem->VisualKits.Contains(SchematicCategory.Icon)) {
			ApplyVisualKitToCategory(Subsystem, *Subsystem->VisualKits.Find(SchematicCategory.Icon), CategoryClass);
		}
	}
}

void UCLCategoryBPFLib::InitSchematicCategoryFromStruct(FContentLib_SchematicCategory SchematicCategory,TSubclassOf<UFGSchematicCategory> SchematicCategoryClass,UContentLibSubsystem *Subsystem)
{
	InitCategoryFromSchematicStruct(SchematicCategory, SchematicCategoryClass, Subsystem);
}

void UCLCategoryBPFLib::InitBuildCategoryFromStruct(FContentLib_SchematicCategory SchematicCategory,TSubclassOf<UFGBuildCategory> BuildCategoryClass,UContentLibSubsystem *Subsystem)
{
	InitCategoryFromSchematicStruct(SchematicCategory, BuildCategoryClass, Subsystem);
}

void UCLCategoryBPFLib::ApplyVisualKitToCategory(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit, TSubclassOf<UFGCategory> CategoryClass)
{
	UFGCategory* Obj = CategoryClass.GetDefaultObject();
	if (!IsValid(Obj)) {
		return;
	}

	if (IsValid(Subsystem) && Subsystem->Icons.Contains(Kit.SmallIcon)) {
		Obj->mCategoryIcon.SetResourceObject(*Subsystem->Icons.Find(Kit.SmallIcon));
	} else {
		// TODO: Why do we try getting the small icon, but set the resource object to big icon?
		if (Kit.GetSmallIcon()) {
			Obj->mCategoryIcon.SetResourceObject(Kit.GetBigIcon());
		}
	}
}

void UCLCategoryBPFLib::ApplyVisualKitToPurchaseCategory(UContentLibSubsystem* Subsystem,FContentLib_VisualKit Kit, TSubclassOf<UFGBuildCategory> Item)
{
	ApplyVisualKitToCategory(Subsystem, Kit, Item);
}

void UCLCategoryBPFLib::ApplyVisualKitToSchematicCategory(UContentLibSubsystem* Subsystem,FContentLib_VisualKit Kit, TSubclassOf<UFGSchematicCategory> Item)
{
	ApplyVisualKitToCategory(Subsystem, Kit, Item);
}
