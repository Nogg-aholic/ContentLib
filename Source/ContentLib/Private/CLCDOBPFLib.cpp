


#include "CLCDOBPFLib.h"
#include "BPFContentLib.h"
#include "Contentlib.h"
#include "Reflection/ReflectionHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
void UCLCDOBPFLib::Log(FString LogString, int32 Level)
{
	if (Level == 0)
	{
		UE_LOG(LogContentLib, Display, TEXT("%s"), *LogString);
	}
	else if (Level == 1)
	{
		UE_LOG(LogContentLib, Warning, TEXT("%s"), *LogString);
	}
	else if (Level == 2)
	{
		UE_LOG(LogContentLib, Error, TEXT("%s"), *LogString);
	}

}

bool UCLCDOBPFLib::GenerateCLCDOFromString(FString String, bool DoLog)
{
	if (UBPFContentLib::FailsBasicJsonFormCheck(String)) {
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*String);
	FJsonSerializer Serializer;
	TSharedPtr<FJsonObject> Result;
	Serializer.Deserialize(Reader, Result);
	if (!Result.IsValid())
	{
		UE_LOG(LogContentLib, Error, TEXT("Invalid Json ! %s"), *String);
		return false;
	}
	UClass* Class = nullptr;
	if(Result->HasField("Class") && Result->Values.Find("Class")->Get()->Type == EJson::String)
	{
		const FString ClassPath = Result->GetStringField("Class");
		UObject* Loaded = LoadObject<UObject>(nullptr, *ClassPath);
		if (Loaded)
		{
			Class = Cast<UClass>(Loaded);
			if (!Class)
			{
				Class = Loaded->GetClass();
			}
			else
			{
				Loaded = Class->GetDefaultObject();;
			}
			if(Result->HasField("Edits") && Result->Values.Find("Edits")->Get()->Type == EJson::Array)
			{
				TArray<TSharedPtr<FJsonValue>> Arr = Result->GetArrayField("Edits");
				for(auto i : Arr)
				{
					if(i->Type != EJson::Object)
					{
						UE_LOG(LogContentLib,Error,TEXT("Property Array Value Type not Object Value"));
					}
					else
					{
						auto Obj = i->AsObject();
						if(Obj->HasField("Property") && Obj->Values.Find("Property")->Get()->Type == EJson::String)
						{
							FString PropertyName = Obj->GetStringField("Property");
							FProperty * Prop = Class->FindPropertyByName(FName(PropertyName));
							if(Prop)
							{
								if(Obj->HasField("Value") && Obj->Values.Find("Value")->Get()->Type != EJson::Null)
								{
									EditCDO(Prop, *Obj->Values.Find("Value"), DoLog, Loaded);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogContentLib, Error, TEXT("Finding by Path %s failed"),  *ClassPath);
		}
	}
	
	return true;
}

void UCLCDOBPFLib::EditCDO(FProperty * Prop, TSharedPtr<FJsonValue> json,bool DoLog,void * Pstr)
{
	void* Ptr = Prop->ContainerPtrToValuePtr<void>(Pstr);

	if (FStrProperty* StrProp = CastField<FStrProperty>(Prop)) {
		const FString currentValue = StrProp->GetPropertyValue(Ptr); 
		if (currentValue != json->AsString())
		{
			if(DoLog)
				Log(FString("Overwrite FStrProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(currentValue).Append(" NewValue = ").Append(json->AsString()),0);
			StrProp->SetPropertyValue(Ptr, json->AsString());
		}
	}
	else if (FTextProperty*  TxtProp = CastField<FTextProperty>(Prop)) {
		const FText CurrentValue = TxtProp->GetPropertyValue(Ptr);
		if (CurrentValue.ToString() != json->AsString())
		{
			if (DoLog)
				Log(FString("Overwrite FStrProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(CurrentValue.ToString()).Append(" NewValue = ").Append(json->AsString()), 0);
			TxtProp->SetPropertyValue(Ptr, FText::FromString(json->AsString()));
		}
	}
	else if (FNameProperty* NameProp = CastField<FNameProperty>(Prop)) {
		const FName CurrentValue = NameProp->GetPropertyValue(Ptr);
		if (CurrentValue.ToString() != json->AsString())
		{
			if (DoLog)
				Log(FString("Overwrite FNameProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(CurrentValue.ToString()).Append(" NewValue = ").Append(json->AsString()), 0);
			NameProp->SetPropertyValue(Ptr, *json->AsString());
		}

	}
	else if (FFloatProperty* FProp = CastField<FFloatProperty>(Prop)) {
		const float CurrentValue = FProp->GetPropertyValue(Ptr);
		if (CurrentValue != json->AsNumber())
		{
			if (DoLog)
				Log(FString("Overwrite FFloatProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(FString::SanitizeFloat(CurrentValue)).Append(" NewValue = ").Append(FString::SanitizeFloat(json->AsNumber())), 0);
			FProp->SetPropertyValue(Ptr, json->AsNumber());
		}
	}
	else if (FIntProperty* IProp = CastField<FIntProperty>(Prop)) {
		const int64 CurrentValue = IProp->GetPropertyValue(Ptr);
		if (CurrentValue != json->AsNumber())
		{
			if (DoLog)
				Log(FString("Overwrite FIntProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(FString::FromInt(CurrentValue)).Append(" NewValue = ").Append(FString::FromInt(json->AsNumber())), 0);
			IProp->SetPropertyValue(Ptr, json->AsNumber());
		}
	}
	else if (FBoolProperty* BProp = CastField<FBoolProperty>(Prop)) {
		const bool bCurrentValue = BProp->GetPropertyValue(Ptr);
		if (bCurrentValue != json->AsBool())
		{
			if (DoLog)
				Log(FString("Overwrite FBoolProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(bCurrentValue ? "true" : "false" ).Append(" NewValue = ").Append(json->AsBool() ? "true" : "false"), 0);
			BProp->SetPropertyValue(Ptr, json->AsBool());
		}
	}
	else if (FEnumProperty* EProp = CastField<FEnumProperty>(Prop)) {	
		if (json->Type == EJson::Number) {
			FString StringInt = FString::FromInt(json->AsNumber());
			FNumericProperty* NumProp = CastField<FNumericProperty>(EProp);
			if (EProp->GetUnderlyingProperty()->GetNumericPropertyValueToString(Ptr) != StringInt) {
				if (DoLog)
					Log(FString("Overwrite FIntProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(EProp->GetUnderlyingProperty()->GetNumericPropertyValueToString(Ptr)).Append(" NewValue = ").Append(StringInt), 0);
				NumProp->SetNumericPropertyValueFromString(Ptr, *StringInt);
			}
		}
	}
	else if (FByteProperty* ByteProp = CastField<FByteProperty>(Prop)) {
		if (ByteProp->IsEnum()) {
			if (json->Type == EJson::Number) {
				auto ByteValue = ByteProp->GetPropertyValue(Ptr);
				uint8 NewValue = json->AsNumber();
				if (NewValue != ByteValue)
				{
					if (DoLog)
						Log(FString("Overwrite FByteProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(FString::FromInt(ByteValue)).Append(" NewValue = ").Append(FString::FromInt(NewValue)), 0);
					ByteProp->SetPropertyValue(Ptr, NewValue);
				}
			}
		}
		else
		{
			if (json->Type == EJson::Number) {
				FNumericProperty* NumProp = CastField<FNumericProperty>(ByteProp);
				if (NumProp && NumProp->GetSignedIntPropertyValue(Ptr) != json->AsNumber()) {
					if (DoLog)
						Log(FString("Overwrite FByteProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(ByteProp->GetNumericPropertyValueToString(Ptr)).Append(" NewValue = ").Append(FString::FromInt(json->AsNumber())), 0);
					ByteProp->SetNumericPropertyValueFromString(Ptr, *FString::FromInt(json->AsNumber()));
				}
			}
		}
		
	}
	else if (FDoubleProperty* DdProp = CastField<FDoubleProperty>(Prop)) {
		if (DdProp->GetPropertyValue(Ptr) != json->AsNumber())
		{
			if (DoLog)
				Log(FString("Overwrite FDoubleProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(DdProp->GetNumericPropertyValueToString(Ptr)).Append(" NewValue = ").Append(FString::SanitizeFloat(json->AsNumber())), 0);
			DdProp->SetPropertyValue(Ptr, json->AsNumber());
		}
	}
	else if (FNumericProperty* NumProp = CastField<FNumericProperty>(Prop)) {
		if (NumProp->GetNumericPropertyValueToString(Ptr) != json->AsString())
		{
			if (DoLog)
				Log(FString("Overwrite FNumericProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(NumProp->GetNumericPropertyValueToString(Ptr)).Append(" NewValue = ").Append(json->AsString()), 0);
			NumProp->SetNumericPropertyValueFromString(Ptr, *json->AsString());
		}
	}
	else if (FArrayProperty* AProp = CastField<FArrayProperty>(Prop)) {
		if (json->Type == EJson::Array)
		{
			FScriptArrayHelper Helper(AProp, Ptr);
			Helper.EmptyValues();
			for(auto i : json->AsArray())
			{
				EditCDO(AProp->Inner,i, DoLog, Helper.GetRawPtr(Helper.AddValue()));
			}	
		}
	}
	else if (FMapProperty* MProp = CastField<FMapProperty>(Prop)) {
		if (json->Type == EJson::Array)
		{
			TArray<TSharedPtr<FJsonValue>> jsonArr = json->AsArray();
			FScriptMapHelper MapHelper(MProp, Ptr);
			TArray<uint32> MapHashes = {};
			TArray<int32> HashesNew = {};
			MapHelper.EmptyValues();
			for (int i = 0; i < jsonArr.Num(); i++) {
				void* PropertyValue = FMemory::Malloc(MProp->KeyProp->GetSize());
				void* ValueValue = FMemory::Malloc(MProp->ValueProp->GetSize());

				MProp->KeyProp->InitializeValue(PropertyValue);
				MProp->ValueProp->InitializeValue(ValueValue);
				TSharedPtr<FJsonValue> KeyValue = jsonArr[i]->AsObject()->TryGetField("CL_Key");
				EditCDO(MProp->KeyProp,KeyValue, DoLog, PropertyValue);
				TSharedPtr<FJsonValue> ObjectValue = jsonArr[i]->AsObject()->TryGetField("CL_Value");
				EditCDO(MProp->ValueProp,ObjectValue, DoLog, ValueValue);
				MapHelper.AddPair(PropertyValue,ValueValue);
				MProp->KeyProp->DestroyValue(PropertyValue);
				FMemory::Free(PropertyValue);
				MProp->ValueProp->DestroyValue(ValueValue);
				FMemory::Free(ValueValue);
			}

			MapHelper.Rehash();
		}
	}
	else if (FObjectProperty* UProp = CastField<FObjectProperty>(Prop)) {
	
		if (json->Type == EJson::String) {

				UObject* Obj = UProp->GetPropertyValue(Ptr);
				if (json->AsString() != "") {
					UObject* UObj = LoadObject<UObject>(nullptr, *json->AsString());
					if (!UObj)
					{
						if (DoLog)
							Log(FString("Skipped Overwrite Reason[Load Fail] FObjectProperty: ").Append(Prop->GetName()).Append(" Value : ").Append((Obj ? Obj->GetPathName() : FString("nullpeter")).Append(" with: ").Append((UObj ? UObj->GetPathName() : FString("nullpeter")))), 0);
					}
					else
					{
						if (DoLog)
							Log(FString("Overwrite FObjectProperty: ").Append(Prop->GetName()).Append(" Value : ").Append((Obj ? Obj->GetPathName() : FString("nullpeter")).Append(" with: ").Append((UObj ? UObj->GetPathName() : FString("nullpeter")))), 0);
						UProp->SetPropertyValue(Ptr, UObj);
					}
				}
				else if (Obj)
				{
					if (DoLog)
						Log(FString("Overwrite FObjectProperty: ").Append(Prop->GetName()).Append(" Value : ").Append(Obj->GetPathName().Append(" with nullpeter")), 0);
					UProp->SetPropertyValue(Ptr, nullptr);
				}
			}
		}		
	else if (FClassProperty* CProp = CastField<FClassProperty>(Prop)) {
		UClass * CastResult = nullptr;
		if (json->Type == EJson::String) {
			const FString ClassPath = json->AsString();
			if (ClassPath == "")
			{
				if (CProp->GetPropertyValue(Ptr) != nullptr)
				{
					if (DoLog)
						Log(FString("Overwrite FClassProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(CProp->GetPropertyValue(Ptr)->GetPathName()).Append(" NewValue = Nullpeter"), 0);
					CProp->SetPropertyValue(Ptr, CastResult);
				}
			}
			else
			{
				CastResult = LoadObject<UClass>(nullptr, *json->AsString());
				// Failsafe for script classes with BP Stubs
				if (!CastResult && json->AsString() != "") {
					CastResult = FSoftClassPath(json->AsString()).TryLoadClass<UClass>();
					if (!CastResult)
					{
						FString ClassName; FString Path; FString Right;
						FPaths::Split(json->AsString(), Path, Right, ClassName);
						if (ClassName != "")
						{
							Log(FString("FailSafe Class #1 Loading ").Append(json->AsString()).Append("--  Searching : ").Append(ClassName), 1);
							UClass * Out = FindClassByName(ClassName);
							if (Out)
								CastResult = Out;

							if (!ClassName.EndsWith("_C"))
							{
								Log(FString("FailSafe Class #2 Loading ").Append(json->AsString()).Append("--  Searching : ").Append(ClassName.Append("_C")), 1);
								CastResult = FindClassByName(ClassName.Append("_C"));
							}
						}
					}
				}
				if (CastResult && CastResult != CProp->GetPropertyValue(Ptr))
				{
					if (DoLog)
						Log(FString("Overwrite FClassProperty: ").Append(Prop->GetName()).Append(" OldValue = ").Append(CProp->GetPropertyValue(Ptr)->GetPathName()).Append(" NewValue = ").Append(CastResult->GetPathName()), 0);
					CProp->SetPropertyValue(Ptr, CastResult);
				}
			}
		}
	}
	else if (FStructProperty* SProp = CastField<FStructProperty>(Prop)) {
		if (json->Type == EJson::Object)
		{
			for (auto prop = TFieldIterator<FProperty>(SProp->Struct); prop; ++prop) {
				FProperty* Prop = *prop;
				FString FieldName = Prop->GetName();
				if (json->AsObject()->HasField(FieldName))
				{
					TSharedPtr<FJsonValue> Jval = *json->AsObject()->Values.Find(FieldName);
					EditCDO(Prop, Jval, DoLog, Ptr);
				}
			}
		}
	}
	else if (FWeakObjectProperty* WeakObjectProperty = CastField<FWeakObjectProperty>(Prop))
	{
		FWeakObjectPtr WObj = WeakObjectProperty->GetPropertyValue(Ptr);

		
		if (json->AsString() != "")
		{
			UObject* UObj = FSoftObjectPath(json->AsString()).TryLoad();
			UObject* Obj = WObj.Get();

			if (DoLog)
				Log(FString("Overwrite WeakObjectProperty: ").Append(Prop->GetName()).Append(" Value : ").Append((Obj ? Obj->GetPathName() : FString("nullpeter")).Append(" with: ").Append((UObj ? UObj->GetPathName() : FString("nullpeter")))), 0);
			WeakObjectProperty->SetPropertyValue(Ptr, UObj);
		}
		else
		{
			if (WObj.IsValid())
			{
				if (DoLog)
					Log(FString("Overwrite FWeakObjectProperty: ").Append(Prop->GetName()).Append(" with nullpeter"), 0);
				WeakObjectProperty->SetPropertyValue(Ptr, nullptr);
			}
		}
	}
	else if (FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Prop))
	{
		if (json->AsString() != "")
		{
			const FString PathString = json->AsString();
			FSoftObjectPtr* ObjectPtr = static_cast<FSoftObjectPtr*>(Ptr);
			*ObjectPtr = FSoftObjectPath(PathString);
		}
	}
}


UClass* UCLCDOBPFLib::FindClassByName(const FString ClassNameInput) {
  	// prevent crash from wrong user Input
  	if (ClassNameInput.Len() == 0)
  		return nullptr;
  
  	const TCHAR* ClassName = *ClassNameInput;
  	UObject* ClassPackage = ANY_PACKAGE;
  
  	if (UClass* Result = FindObject<UClass>(ANY_PACKAGE, ClassName, false))
  		return Result;
  
  	if (UObjectRedirector* RenamedClassRedirect = FindObject<UObjectRedirector>(ANY_PACKAGE, ClassName, true))
  		return CastChecked<UClass>(RenamedClassRedirect->DestinationObject);
  
  	return nullptr;
  }