


#include "CLItemBPFLib.h"
#include "BPFContentLib.h"
#include "FGWorldSettings.h"
#include "ContentLib.h"
#include "Serialization/JsonSerializer.h"




FContentLib_VisualKit::FContentLib_VisualKit()
{
	FluidColor = FColor();
	GasColor = FColor();
}

UTexture2D* FContentLib_VisualKit::GetSmallIcon() const
{
	return LoadObject<UTexture2D>(nullptr, *SmallIcon);
}

UTexture2D* FContentLib_VisualKit::GetBigIcon() const
{
	return LoadObject<UTexture2D>(nullptr, *BigIcon);
}

UStaticMesh* FContentLib_VisualKit::GetMesh() const
{
	return LoadObject<UStaticMesh>(nullptr, *Mesh);
}

template<typename ColorType>
static TSharedPtr<FJsonObject> ColorAsJsonObject(const ColorType& Color)
{
	auto ColorJson = MakeShared<FJsonObject>();
	ColorJson->Values.Add("r", MakeShared<FJsonValueNumber>(Color.R));
	ColorJson->Values.Add("g", MakeShared<FJsonValueNumber>(Color.G));
	ColorJson->Values.Add("b", MakeShared<FJsonValueNumber>(Color.B));
	ColorJson->Values.Add("a", MakeShared<FJsonValueNumber>(Color.A));
	return ColorJson;
}

TSharedRef<FJsonObject> FContentLib_VisualKit::GetAsJsonObject(const TSubclassOf<UFGItemDescriptor> Item)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return Obj;
	}

	const auto Mesh = MakeShared<FJsonValueString>(CDO->GetItemMesh(Item)->GetPathName());
	const auto BigIcon = MakeShared<FJsonValueString>(CDO->GetBigIcon(Item)->GetPathName());
	const auto SmallIcon = MakeShared<FJsonValueString>(CDO->GetSmallIcon(Item)->GetPathName());
	const auto Color = MakeShared<FJsonValueObject>(ColorAsJsonObject(CDO->GetFluidColor(Item)));
	const auto ColorGas = MakeShared<FJsonValueObject>(ColorAsJsonObject(CDO->GetGasColor(Item)));

	Obj->Values.Add("Mesh", Mesh);
	Obj->Values.Add("BigIcon", BigIcon);
	Obj->Values.Add("SmallIcon", SmallIcon);
	Obj->Values.Add("FluidColor", Color);
	Obj->Values.Add("GasColor", ColorGas);

	return Obj;
}



FContentLib_ResourceItem::FContentLib_ResourceItem(): CollectSpeedMultiplier(-1)
{
	PingColor = FColor(0, 0, 0, 0);
}

TSharedRef<FJsonObject> FContentLib_ResourceItem::GetResourceAsJsonObject(TSubclassOf<UFGResourceDescriptor> Item)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return Obj;
	}

	const auto Color = MakeShared<FJsonValueObject>(ColorAsJsonObject(CDO->GetPingColor(Item)));
	const auto CollectSpeedMultiplier = MakeShared<FJsonValueNumber>(CDO->GetCollectSpeedMultiplier(Item));

	Obj->Values.Add("PingColor", Color);
	Obj->Values.Add("CollectSpeedMultiplier", CollectSpeedMultiplier);

	return Obj;
}



FContentLib_NuclearFuelItem::FContentLib_NuclearFuelItem(): AmountOfWaste(-1)
{
}

TSharedRef<FJsonObject> FContentLib_NuclearFuelItem::GetNuclearFuelAsJsonObject(TSubclassOf<UFGItemDescriptorNuclearFuel> Item)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return Obj;
	}

	if (const auto spentFuelItem = CDO->GetSpentFuelClass(Item)) {
		const auto SpentFuelClass = MakeShared<FJsonValueString>(spentFuelItem->GetPathName());
		Obj->Values.Add("SpentFuelClass", SpentFuelClass);
	}
	
	const auto AmountOfWaste = MakeShared<FJsonValueNumber>(CDO->GetAmountWasteCreated(Item));
	Obj->Values.Add("AmountOfWaste", AmountOfWaste);

	return Obj;
}


FContentLib_Item::FContentLib_Item():
	Form(EResourceForm::RF_LAST_ENUM),
	StackSize(EStackSize::SS_LAST_ENUM),
	EnergyValue(-1),
	RadioactiveDecay(-1),
	CanBeDiscarded(-1),
	RememberPickUp(-1),
	ResourceSinkPoints(-1),
	FuelWasteItem()
{
}

static FString GetFormString(EResourceForm ResourceForm, bool SuppressLastEnumError = false)
{
	using enum EResourceForm;

	switch (ResourceForm) {
	case RF_SOLID:
		return "Solid";
	case RF_LIQUID:
		return "Liquid";
	case RF_GAS:
		return "Gas";
	case RF_HEAT:
		return "Heat";
	case RF_INVALID:
		return "Invalid";
	case RF_LAST_ENUM: // This is used to specify no form change in patches so it's okay sometimes
		if (!SuppressLastEnumError) {
			UE_LOG(LogContentLib, Error, TEXT("Encountered EResourceForm::RF_LAST_ENUM, should be impossible"));
		}
		return "Unknown";
	default:
		return "Unknown"; // Missing case
	}
}

static FString GetSizeString(EStackSize StackSize, bool SuppressLastEnumError = false)
{
	using enum EStackSize;

	switch (StackSize) {
	case SS_ONE:
		return "One";
	case SS_SMALL:
		return "Small";
	case SS_MEDIUM:
		return "Medium";
	case SS_BIG:
		return "Big";
	case SS_HUGE:
		return "Huge";
	case SS_FLUID:
		return "Fluid";
	case SS_LAST_ENUM: // This is used to specify no stack size change in patches so it's okay sometimes
		if (!SuppressLastEnumError) {
			UE_LOG(LogContentLib, Error, TEXT("Encountered EStackSize::SS_LAST_ENUM, should be impossible"));
		}
		return "Unknown";
	default:
		return "Unknown"; // Missing case
	}
}

FString UCLItemBPFLib::GenerateStringFromCLItem(FContentLib_Item Item)
{
	// TODO remove code duplication here
	const auto CDO = Item;
	const auto Obj = MakeShared<FJsonObject>();

	const FString FormString = GetFormString(CDO.Form, true);
	const FString SizeString = GetSizeString(CDO.StackSize, true);

	if (FormString != "Unknown") {
		const auto Form = MakeShared<FJsonValueString>(FormString);
		Obj->Values.Add("Form", Form);
	}

	if (SizeString != "Unknown") {
		const auto Size = MakeShared<FJsonValueString>(SizeString);
		Obj->Values.Add("StackSize", Size);
	}

	if (CDO.Name != "") {
		const auto Name = MakeShared<FJsonValueString>(CDO.Name);
		Obj->Values.Add("Name", Name);
	}

	if (CDO.NameShort != "") {
		const auto NameShort = MakeShared<FJsonValueString>(CDO.NameShort);
		Obj->Values.Add("NameShort", NameShort);
	}

	if (CDO.Description != "") {
		const auto Description = MakeShared<FJsonValueString>(CDO.Description);
		Obj->Values.Add("Description", Description);
	}
	if (CDO.Category != "") {
		const auto Category = MakeShared<FJsonValueString>(CDO.Category);
		Obj->Values.Add("Category", Category);
	}

	if (CDO.VisualKit != "") {
		const auto ItemCategory = MakeShared<FJsonValueString>(CDO.VisualKit);
		Obj->Values.Add("VisualKit", ItemCategory);
	}

	if (CDO.EnergyValue != -1) {
		const auto EnergyValue = MakeShared<FJsonValueNumber>(CDO.EnergyValue);
		Obj->Values.Add("EnergyValue", EnergyValue);
	}

	if (CDO.RadioactiveDecay != -1) {
		const auto RadioactiveDecay = MakeShared<FJsonValueNumber>(CDO.RadioactiveDecay);
		Obj->Values.Add("RadioactiveDecay", RadioactiveDecay);
	}

	if (CDO.CanBeDiscarded != -1) {
		const auto CanBeDiscarded = MakeShared<FJsonValueBoolean>(static_cast<bool>(CDO.CanBeDiscarded));
		Obj->Values.Add("CanBeDiscarded", CanBeDiscarded);
	}
	if (CDO.RememberPickUp != -1) {
		const auto RememberPickUp = MakeShared<FJsonValueBoolean>(static_cast<bool>(CDO.RememberPickUp));
		Obj->Values.Add("RememberPickUp", RememberPickUp);
	}

	if (CDO.ResourceSinkPoints != -1) {
		const auto ResourceSinkPoints = MakeShared<FJsonValueNumber>(CDO.ResourceSinkPoints);
		Obj->Values.Add("ResourceSinkPoints", ResourceSinkPoints);
	}

	if (CDO.ResourceItem.CollectSpeedMultiplier != -1 || CDO.ResourceItem.PingColor != FColor(0,0,0,0)) {
		const auto Objx = MakeShared<FJsonObject>();

		if (CDO.ResourceItem.PingColor != FColor(0, 0, 0, 0)) {
			const auto Color = MakeShared<FJsonValueObject>(ColorAsJsonObject(CDO.ResourceItem.PingColor));
			Objx->Values.Add("PingColor", Color);
		}

		if (CDO.ResourceItem.CollectSpeedMultiplier != -1) {
			const auto CollectSpeedMultiplier = MakeShared<FJsonValueNumber>(CDO.ResourceItem.CollectSpeedMultiplier);
			Objx->Values.Add("CollectSpeedMultiplier", CollectSpeedMultiplier);
		}

		Obj->Values.Add("ResourceItem", MakeShared<FJsonValueObject>(Objx));
	}

	if (CDO.FuelWasteItem.SpentFuelClass != "" || CDO.FuelWasteItem.AmountOfWaste != -1) {
		const auto Objx = MakeShared<FJsonObject>();

		if (CDO.FuelWasteItem.SpentFuelClass != "") {
			const auto SpentFuelClass = MakeShared<FJsonValueString>(CDO.FuelWasteItem.SpentFuelClass);
			Objx->Values.Add("SpentFuelClass", SpentFuelClass);
		}

		if (CDO.FuelWasteItem.AmountOfWaste != -1) {
			const auto AmountOfWaste = MakeShared<FJsonValueNumber>(CDO.FuelWasteItem.AmountOfWaste);
			Obj->Values.Add("AmountOfWaste", AmountOfWaste);
		}

		Obj->Values.Add("FuelWasteItem", MakeShared<FJsonValueObject>(Objx));
	}

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<
		TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FString UCLItemBPFLib::GenerateFromDescriptorClass(TSubclassOf<UFGItemDescriptor> Item)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return "";
	}

	const FString FormString = GetFormString(CDO->mForm);
	const FString SizeString = GetSizeString(CDO->mStackSize);
	
	const auto Form = MakeShared<FJsonValueString>(FormString);
	const auto Size = MakeShared<FJsonValueString>(SizeString);
	Obj->Values.Add("Form", Form);
	Obj->Values.Add("StackSize", Size);

	if (CDO->mDisplayName.ToString() != "") {
		const auto Name = MakeShared<FJsonValueString>(CDO->mDisplayName.ToString());
		Obj->Values.Add("Name", Name);
	}

	if (CDO->mAbbreviatedDisplayName.ToString() != "") {
		const auto NameShort = MakeShared<FJsonValueString>(CDO->mAbbreviatedDisplayName.ToString());
		Obj->Values.Add("NameShort", NameShort);
	}

	if (CDO->mDescription.ToString() != "") {
		const auto Description = MakeShared<FJsonValueString>(CDO->mDescription.ToString());
		Obj->Values.Add("Description", Description);
	}

	if (CDO->mCategory) {
		const auto Category = MakeShared<FJsonValueString>(CDO->mCategory->GetPathName());
		Obj->Values.Add("Category", Category);
	}

	const auto VisualKit = MakeShared<FJsonValueObject>(FContentLib_VisualKit::GetAsJsonObject(Item));
	Obj->Values.Add("VisualKit", VisualKit);

	if (CDO->mEnergyValue != 0) {
		const auto EnergyValue = MakeShared<FJsonValueNumber>(CDO->mEnergyValue);
		Obj->Values.Add("EnergyValue", EnergyValue);
	}

	if (CDO->mRadioactiveDecay != 0) {
		const auto RadioactiveDecay = MakeShared<FJsonValueNumber>(CDO->mRadioactiveDecay);
		Obj->Values.Add("RadioactiveDecay", RadioactiveDecay);
	}

	if (CDO->mCanBeDiscarded) {
		const auto CanBeDiscarded = MakeShared<FJsonValueBoolean>(CDO->mCanBeDiscarded);
		Obj->Values.Add("CanBeDiscarded", CanBeDiscarded);
	}

	if (CDO->mRememberPickUp) {
		const auto RememberPickUp = MakeShared<FJsonValueBoolean>(CDO->mRememberPickUp);
		Obj->Values.Add("RememberPickUp", RememberPickUp);
	}

	if (CDO->mResourceSinkPoints != 0) {
		const auto ResourceSinkPoints = MakeShared<FJsonValueNumber>(CDO->mResourceSinkPoints);
		Obj->Values.Add("ResourceSinkPoints", ResourceSinkPoints);
	}

	if (TSubclassOf<UFGResourceDescriptor> Resource = *Item) {
		const auto ResourceItem = MakeShared<FJsonValueObject>(FContentLib_ResourceItem::GetResourceAsJsonObject(Resource));
		Obj->Values.Add("ResourceItem", ResourceItem);
	}

	if (TSubclassOf<UFGItemDescriptorNuclearFuel> NuclearFuel = *Item) {
		const auto FuelWasteItem = MakeShared<FJsonValueObject>(FContentLib_NuclearFuelItem::GetNuclearFuelAsJsonObject(NuclearFuel));
		Obj->Values.Add("FuelWasteItem", FuelWasteItem);
	}

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<
		TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

FContentLib_Item UCLItemBPFLib::GenerateCLItemFromString(FString jsonString)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(jsonString)) {
		return FContentLib_Item();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*jsonString);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
		return FContentLib_Item();

	auto Item = FContentLib_Item();
	if (Result->HasField("Form") && Result->TryGetField("Form")->Type == EJson::String)
	{
		const FString CS = Result->TryGetField("Form")->AsString();
		if (CS.Equals("Solid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_SOLID;
		}
		else if (CS.Equals("Liquid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_LIQUID;
		}
		else if (CS.Equals("Gas", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_GAS;
		}
		else if (CS.Equals("Heat", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_HEAT;
		}
		else if (CS.Equals("Invalid", ESearchCase::IgnoreCase))
		{
			Item.Form = EResourceForm::RF_INVALID;
		}
		else {
			UE_LOG(LogContentLib, Error, TEXT("Unrecognized Form: '%s', falling back to default"), *CS);
		}
	}


	if (Result->HasField("StackSize") && Result->TryGetField("StackSize")->Type == EJson::String)
	{
		const FString CS = Result->TryGetField("StackSize")->AsString();
		if (CS.Equals("One", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_ONE;
		}
		else if (CS.Equals("Small", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_SMALL;
		}
		else if (CS.Equals("Medium", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_MEDIUM;
		}
		else if (CS.Equals("Big", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_BIG;
		}
		else if (CS.Equals("Huge", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_HUGE;
		}
		else if (CS.Equals("Fluid", ESearchCase::IgnoreCase))
		{
			Item.StackSize = EStackSize::SS_FLUID;
		}
		else if (CS.Equals("Liquid", ESearchCase::IgnoreCase))
		{
			UE_LOG(LogContentLib, Error, TEXT("Tried to register an Item with the StackSize of 'Liquid', this is not a real stack size, but treating it as 'Fluid' to avoid in-game weirdness. Please change it to 'Fluid' in your code. Encountered while processing json: %s"), *jsonString);
			Item.StackSize = EStackSize::SS_FLUID;
		}
		else {
			UE_LOG(LogContentLib, Error, TEXT("Unrecognized StackSize: '%s', falling back to default. Encountered while processing json: %s"), *CS, *jsonString);
		}
	}
	UBPFContentLib::SetStringFieldWithLog(Item.Name, "Name", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.NameShort, "NameShort", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.Description, "Description", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.Category, "Category", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.VisualKit, "VisualKit", Result);
	UBPFContentLib::SetFloatFieldWithLog(Item.EnergyValue, "EnergyValue", Result);
	UBPFContentLib::SetFloatFieldWithLog(Item.RadioactiveDecay, "RadioactiveDecay", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Item.CanBeDiscarded, "CanBeDiscarded", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Item.RememberPickUp, "RememberPickUp", Result);
	UBPFContentLib::SetIntegerFieldWithLog(Item.ResourceSinkPoints, "ResourceSinkPoints", Result);


	if (Result->HasField("ResourceItem") && Result->TryGetField("ResourceItem")->Type == EJson::Object)
	{
		const auto OBJ = Result->TryGetField("ResourceItem")->AsObject();
		UBPFContentLib::SetLinearColorFieldWithLog(Item.ResourceItem.PingColor, "PingColor", OBJ);
		UBPFContentLib::SetFloatFieldWithLog(Item.ResourceItem.CollectSpeedMultiplier, "CollectSpeedMultiplier", OBJ);
	}

	if (Result->HasField("FuelWasteItem") && Result->TryGetField("FuelWasteItem")->Type == EJson::Object)
	{
		const auto OBJ = Result->TryGetField("FuelWasteItem")->AsObject();
		UBPFContentLib::SetStringFieldWithLog(Item.FuelWasteItem.SpentFuelClass, "SpentFuelClass", Result);
		UBPFContentLib::SetIntegerFieldWithLog(Item.FuelWasteItem.AmountOfWaste, "AmountOfWaste", Result);
	}
	return Item;
}

FContentLib_VisualKit UCLItemBPFLib::GenerateKitFromString(FString String)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return FContentLib_VisualKit();
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
	{
		UE_LOG(LogContentLib, Error, TEXT("Invalid Json ! %s"), *String);
		return FContentLib_VisualKit();
	}

	FContentLib_VisualKit Item;

	UBPFContentLib::SetStringFieldWithLog(Item.Mesh, "Mesh", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.BigIcon, "BigIcon", Result);
	UBPFContentLib::SetStringFieldWithLog(Item.SmallIcon, "SmallIcon", Result);
	UBPFContentLib::SetColorFieldWithLog(Item.FluidColor, "FluidColor", Result);
	UBPFContentLib::SetColorFieldWithLog(Item.GasColor, "GasColor", Result);

	return Item;
}

FString UCLItemBPFLib::GenerateKitFromClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!Item)
		return "";

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<
		TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(FContentLib_VisualKit::GetAsJsonObject(Item), JsonWriter);
	return Write;

}

FString UCLItemBPFLib::GetVisualKitAsString(FContentLib_VisualKit Kit)
{
	const auto Obj = MakeShared<FJsonObject>();
	const auto Mesh = MakeShared<FJsonValueString>(Kit.Mesh);
	const auto BigIcon = MakeShared<FJsonValueString>(Kit.BigIcon);
	const auto SmallIcon = MakeShared<FJsonValueString>(Kit.SmallIcon);
	const auto Color = MakeShared<FJsonValueObject>(ColorAsJsonObject(Kit.FluidColor));
	const auto ColorGas = MakeShared<FJsonValueObject>(ColorAsJsonObject(Kit.GasColor));

	Obj->Values.Add("Mesh", Mesh);
	Obj->Values.Add("BigIcon", BigIcon);
	Obj->Values.Add("SmallIcon", SmallIcon);
	Obj->Values.Add("FluidColor", Color);
	Obj->Values.Add("GasColor", ColorGas);

	FString Write;
	const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
	FJsonSerializer::Serialize(Obj, JsonWriter);
	return Write;
}

void UCLItemBPFLib::InitItemFromStruct(const TSubclassOf<UFGItemDescriptor> Item, FContentLib_Item ItemStruct, UContentLibSubsystem* Subsystem)
{
	const auto CDO = Item.GetDefaultObject();
	if (!IsValid(CDO)) {
		return;
	}
	
	// Must set this to -1 or it will default to 0, causing the item to have a stack size of 0
	CDO->mCachedStackSize = -1;

	if (ItemStruct.Form != EResourceForm::RF_LAST_ENUM) {
		CDO->mForm = ItemStruct.Form;
	}

	if (ItemStruct.StackSize != EStackSize::SS_LAST_ENUM) {
		CDO->mStackSize = ItemStruct.StackSize;
	}

	if (ItemStruct.Name != "") {
		CDO->mDisplayName = FText::FromString(ItemStruct.Name);
		CDO->mUseDisplayNameAndDescription = true;
	}

	if (ItemStruct.NameShort != "") {
		CDO->mAbbreviatedDisplayName = FText::FromString(ItemStruct.NameShort);
	}

	if (ItemStruct.Description != "") {
		CDO->mDescription = FText::FromString(ItemStruct.Description);
	}

	if (ItemStruct.Category != "") {
		if (TSubclassOf<UFGCategory> Out = UBPFContentLib::SetCategoryWithLoad(*ItemStruct.Category, Subsystem, false)) {
			CDO->mCategory = Out;
		} else {
			UE_LOG(LogContentLib, Error, TEXT("Unrecognized Category: '%s' that failed to be created via SetCategoryWithLoad"), *ItemStruct.Category)
		}
		if (!CDO->mCategory) {
			UE_LOG(LogContentLib, Error, TEXT("Item Category probably failed; a category was specified in the struct but was still nullptr after apply"));
		}
	}
	if (!CDO->mCategory) {
		UE_LOG(LogContentLib, Warning, TEXT("Item Category is blank, this means recipes for it won't show up unless searched for, or they have a category override"));
	}

	if (ItemStruct.VisualKit != "") {
		if (ItemStruct.VisualKit.Contains("{") && ItemStruct.VisualKit.Contains("}")) {
			FContentLib_VisualKit Kit = GenerateKitFromString(ItemStruct.VisualKit);
			ApplyVisualKitToItem(Subsystem, Kit, Item);
		} else {
			if (Subsystem->ImportedVisualKits.Contains(ItemStruct.VisualKit)) {
				ApplyVisualKitToItem(Subsystem, *Subsystem->ImportedVisualKits.Find(ItemStruct.VisualKit), Item);
			} else if (Subsystem->VisualKits.Contains(ItemStruct.VisualKit)) {
				ApplyVisualKitToItem(Subsystem, *Subsystem->VisualKits.Find(ItemStruct.VisualKit), Item);
			}
		}
	}

	// TODO negative energy values may have use in the future (coolant?) so maybe don't do this
	if (ItemStruct.EnergyValue != -1) {
		CDO->mEnergyValue = ItemStruct.EnergyValue;
	}
	if (ItemStruct.RadioactiveDecay != -1) {
		CDO->mRadioactiveDecay = ItemStruct.RadioactiveDecay;
	}
	if (ItemStruct.CanBeDiscarded != -1) {
		CDO->mCanBeDiscarded = static_cast<bool>(ItemStruct.CanBeDiscarded);
	}
	if (ItemStruct.RememberPickUp != -1) {
		CDO->mRememberPickUp = static_cast<bool>(ItemStruct.RememberPickUp);
	}
	if (ItemStruct.ResourceSinkPoints != -1) {
		// TODO this writes to a cache field, to actually overwrite points we need to talk to the subsystem
		CDO->mResourceSinkPoints = ItemStruct.ResourceSinkPoints;
	}

	if (const auto ResourceCDO = Cast<UFGResourceDescriptor>(CDO)) {
		if (ItemStruct.ResourceItem.PingColor != FColor(0, 0, 0, 0)) {
			ResourceCDO->mPingColor = ItemStruct.ResourceItem.PingColor;
		}
		if (ItemStruct.ResourceItem.CollectSpeedMultiplier != -1) {
			ResourceCDO->mCollectSpeedMultiplier = ItemStruct.ResourceItem.CollectSpeedMultiplier;
		}
	}

	if (const auto NuclearFuelCDO = Cast<UFGItemDescriptorNuclearFuel>(CDO)) {
		if (ItemStruct.FuelWasteItem.SpentFuelClass != "") {
			if (ItemStruct.FuelWasteItem.SpentFuelClass.Contains("/")) {
				UClass* Loaded = LoadObject<UClass>(nullptr, *ItemStruct.FuelWasteItem.SpentFuelClass);
				if (Loaded && Loaded->IsChildOf(UFGItemCategory::StaticClass())) {
					NuclearFuelCDO->mSpentFuelClass = Loaded;
				}
			} else {
				for (auto e : Subsystem->mItems) {
					if (UBPFContentLib::StringCompareItem(e->GetName(),ItemStruct.FuelWasteItem.SpentFuelClass,"Desc","_C")) {
						NuclearFuelCDO->mSpentFuelClass = e;
						break;
					}
				}
			}
		}

		if (ItemStruct.FuelWasteItem.AmountOfWaste != -1) {
			NuclearFuelCDO->mAmountOfWaste = ItemStruct.FuelWasteItem.AmountOfWaste;
		}
	}
}



FString UCLItemBPFLib::GenerateFromNuclearFuelClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (TSubclassOf<UFGItemDescriptorNuclearFuel> NuclearFuel = *Item) {
		FString Write;
		const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<
			TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
		FJsonSerializer::Serialize(FContentLib_NuclearFuelItem::GetNuclearFuelAsJsonObject(NuclearFuel), JsonWriter);
		return Write;
	} else {
		return "";
	}
}



FString UCLItemBPFLib::GenerateResourceFromClass(TSubclassOf<UFGItemDescriptor> Item)
{
	if (TSubclassOf<UFGResourceDescriptor> Resource = *Item) {
		FString Write;
		const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<
			TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Write); //Our Writer Factory
		FJsonSerializer::Serialize(FContentLib_ResourceItem::GetResourceAsJsonObject(Resource), JsonWriter);
		return Write;
	} else {
		return "";
	}
}

void UCLItemBPFLib::ApplyVisualKitToItem(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit, TSubclassOf<UFGItemDescriptor> Item)
{
	const auto Obj = Item.GetDefaultObject();
	if (!IsValid(Obj)) {
		return;
	}

	if (Kit.GetMesh()) {
		Obj->mConveyorMesh = Kit.GetMesh();
	}

	if (IsValid(Subsystem) && Subsystem->Icons.Contains(Kit.BigIcon)) {
		Obj->mPersistentBigIcon = *Subsystem->Icons.Find(Kit.BigIcon);
	} else if (Kit.GetBigIcon()) {
		Obj->mPersistentBigIcon = Kit.GetBigIcon();
	}
	
	if (IsValid(Subsystem) && Subsystem->Icons.Contains(Kit.SmallIcon)) {
		Obj->mSmallIcon = *Subsystem->Icons.Find(Kit.SmallIcon);
	} else if (Kit.GetSmallIcon()) {
		Obj->mSmallIcon = Kit.GetSmallIcon();
	}

	if (Kit.FluidColor != FColor(0, 0, 0, 0)) {
		Obj->mFluidColor = Kit.FluidColor;
	}

	if (Kit.GasColor != FColor(0, 0, 0, 0)) {
		Obj->mGasColor = Kit.GasColor;
	}
}


