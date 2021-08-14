


#include "CLCategoryBPFLib.h"


#include "BPFContentLib.h"
#include "FGBuildCategory.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

FString UCLCategoryBPFLib::GenerateFromItemCategory(TSubclassOf<UFGItemCategory> Item)
{

	const auto CDO = Cast<UFGItemCategory>(Item->GetDefaultObject());
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto MenuPriority = MakeShared<FJsonValueNumber>(CDO->mMenuPriority);
	const auto Obj = MakeShared<FJsonObject>();

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("MenuPriority", MenuPriority);
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
	
}

FContentLib_ItemCategory UCLCategoryBPFLib::CLItemCategoryFromString(FString String)
{
	if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
	{
		if (String == "")
			UE_LOG(LogTemp, Error, TEXT("Empty String  %s"), *String)
		else if (!String.StartsWith("{"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt start with '{' %s"), *String)
		else if (!String.EndsWith("}"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt end with '}'  %s"), *String);

		return FContentLib_ItemCategory();
	}
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_ItemCategory();
	}

	FContentLib_ItemCategory Cat;

	UBPFContentLib::SetStringFieldWithLog(Cat.Name, "Name", Result);
	UBPFContentLib::SetFloatFieldWithLog(Cat.MenuPriority, "MenuPriority", Result);

	return Cat;
	
}

FString UCLCategoryBPFLib::GenerateFromSchematicCategory(TSubclassOf<UFGSchematicCategory> Item)
{

	const auto CDO = Cast<UFGSchematicCategory>(Item->GetDefaultObject());
	const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
	const auto Icon = MakeShared<FJsonValueString>(CDO->mCategoryIcon.GetResourceObject() ? CDO->mCategoryIcon.GetResourceObject()->GetPathName() : "");

	const auto Obj = MakeShared<FJsonObject>();

	Obj->Values.Add("Name",Name);
	Obj->Values.Add("Icon", Icon);
	FString Write;
	const TSharedRef<TJsonWriter<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>> JsonWriter = TJsonWriterFactory<wchar_t, TPrettyJsonPrintPolicy<wchar_t>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FContentLib_SchematicCategory UCLCategoryBPFLib::CLSchematicCategoryFromString(FString String,UContentLibSubsystem* Subsystem)
{

	
	if (String == "" || !String.StartsWith("{") || !String.EndsWith("}"))
	{
		if (String == "")
			UE_LOG(LogTemp, Error, TEXT("Empty String  %s"), *String)
		else if (!String.StartsWith("{"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt start with '{' %s"), *String)
		else if (!String.EndsWith("}"))
			UE_LOG(LogTemp, Error, TEXT("String doesnt end with '}'  %s"), *String);

		return FContentLib_SchematicCategory();
	}
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if(!Result.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Json ! %s"), *String);
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
	
	if(SchematicCategory.Name != "")
		CDO->mDisplayName = FText::FromString(SchematicCategory.Name);

	if(SchematicCategory.MenuPriority != -1)
	{
		CDO->mMenuPriority = SchematicCategory.MenuPriority;
	}
}

void UCLCategoryBPFLib::InitSchematicCategoryFromStruct(FContentLib_SchematicCategory SchematicCategory,TSubclassOf<UFGSchematicCategory> SchematicCategoryClass,UContentLibSubsystem *Subsystem)
{
	UFGSchematicCategory * CDO = SchematicCategoryClass.GetDefaultObject();
	
	if(SchematicCategory.Name != "")
		CDO->mDisplayName = FText::FromString(SchematicCategory.Name);

	if(SchematicCategory.Icon != "")
	{
		if (Subsystem->ImportedVisualKits.Contains(SchematicCategory.Icon))
			ApplyVisualKitToSchematicCategory(Subsystem,*Subsystem->ImportedVisualKits.Find(SchematicCategory.Icon),SchematicCategoryClass);
		else if (Subsystem->VisualKits.Contains(SchematicCategory.Icon))
			ApplyVisualKitToSchematicCategory(Subsystem,*Subsystem->VisualKits.Find(SchematicCategory.Icon),SchematicCategoryClass);	
	}
}

void UCLCategoryBPFLib::InitBuildCategoryFromStruct(FContentLib_SchematicCategory SchematicCategory,TSubclassOf<UFGBuildCategory> BuildCategoryClass,UContentLibSubsystem *Subsystem)
{
	UFGBuildCategory * CDO = BuildCategoryClass.GetDefaultObject();
	
	if(SchematicCategory.Name != "")
		CDO->mDisplayName = FText::FromString(SchematicCategory.Name);

	if(SchematicCategory.Icon != "")
	{
		if (Subsystem->ImportedVisualKits.Contains(SchematicCategory.Icon))
			ApplyVisualKitToPurchaseCategory(Subsystem,*Subsystem->ImportedVisualKits.Find(SchematicCategory.Icon),BuildCategoryClass);
		else if (Subsystem->VisualKits.Contains(SchematicCategory.Icon))
			ApplyVisualKitToPurchaseCategory(Subsystem,*Subsystem->VisualKits.Find(SchematicCategory.Icon),BuildCategoryClass);
	}
}


void UCLCategoryBPFLib::ApplyVisualKitToPurchaseCategory(UContentLibSubsystem* Subsystem,FContentLib_VisualKit Kit, TSubclassOf<UFGBuildCategory> Item)
{
	if(!Item)
		return;
	auto* Obj = Item.GetDefaultObject();
	
	if(Subsystem)
	{
		if(Subsystem->Icons.Contains(Kit.SmallIcon))
		{
			Obj->mCategoryIcon.SetResourceObject(*Subsystem->Icons.Find(Kit.SmallIcon));
		}
		else
		{
			if (Kit.GetSmallIcon())
				Obj->mCategoryIcon.SetResourceObject(Kit.GetBigIcon());
		}
	}
	else
	{
		if (Kit.GetSmallIcon())
			Obj->mCategoryIcon.SetResourceObject(Kit.GetBigIcon());
	}
}

void UCLCategoryBPFLib::ApplyVisualKitToSchematicCategory(UContentLibSubsystem* Subsystem,FContentLib_VisualKit Kit, TSubclassOf<UFGSchematicCategory> Item)
{
	if(!Item)
		return;
	auto* Obj = Item.GetDefaultObject();
	
	if(Subsystem)
	{
		if(Subsystem->Icons.Contains(Kit.SmallIcon))
		{
			Obj->mCategoryIcon.SetResourceObject(*Subsystem->Icons.Find(Kit.SmallIcon));
		}
		else
		{
			if (Kit.GetSmallIcon())
				Obj->mCategoryIcon.SetResourceObject(Kit.GetBigIcon());
		}
	}
	else
	{
		if (Kit.GetSmallIcon())
			Obj->mCategoryIcon.SetResourceObject(Kit.GetBigIcon());
	}
}
