

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ContentLib.h"
#include "ContentLibSubsystem.h"
#include "FGItemCategory.h"
#include "FGRecipe.h"
#include "FGSchematic.h"
#include "ItemAmount.h"
#include "Dom/JsonObject.h"
#include "Serialization/ObjectWriter.h"
#include "Serialization/ObjectReader.h" 
#include "Kismet/BlueprintAssetHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "BPFContentLib.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FObjectFunctionBind, UObject* , Object);

template<typename Tag, typename Tag::type M>
struct Grab {
	friend typename Tag::type get(Tag) {
		return M;
	}
};

struct grab_FuncTable {
	typedef TMap<FName, TObjectPtr<UFunction>> UClass::* type;
	friend type get(grab_FuncTable);
};

template struct Grab<grab_FuncTable, &UClass::FuncMap>;

/**
 * 
 */
UCLASS()
class CONTENTLIB_API UBPFContentLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


		// IO Functions originally by Rama!
		template <class FunctorType>
	class PlatformFileFunctor : public IPlatformFile::FDirectoryVisitor	//GenericPlatformFile.h
	{
	public:

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			return Functor(FilenameOrDirectory, bIsDirectory);
		}

		PlatformFileFunctor(FunctorType&& FunctorInstance)
			: Functor(MoveTemp(FunctorInstance))
		{
		}

	private:
		FunctorType Functor;
	};

	template <class Functor>
	static PlatformFileFunctor<Functor> MakeDirectoryVisitor(Functor&& FunctorInstance)
	{
		return PlatformFileFunctor<Functor>(MoveTemp(FunctorInstance));
	}


public:
	static void SetBooleanFieldWithLog(bool& Field, FString FieldName, TSharedPtr<FJsonObject>   Result);
	static void SetFloatFieldWithLog(float& Field, FString FieldName, TSharedPtr<FJsonObject>   Result);
	static void SetIntegerFieldWithLog(int32& Field, FString FieldName,  TSharedPtr<FJsonObject>  Result);
	static void SetSmallIntegerFieldWithLog(uint8& Field, const FString FieldName, TSharedPtr<FJsonObject> Result);
	static void SetStringFieldWithLog(FString& Field, FString FieldName,  TSharedPtr<FJsonObject>  Result);
	static bool SetStringArrayFieldWithLog(TArray<FString>& Field, FString FieldName,  TSharedPtr<FJsonObject>  Result);
	static bool SetStringIntMapFieldWithLog(TMap<FString,int32>& Field, FString FieldName,  TSharedPtr<FJsonObject>  Result);
	static bool SetColorFieldWithLog(FColor& Field, FString FieldName, TSharedPtr<FJsonObject> Result);
	static bool SetLinearColorFieldWithLog(FLinearColor& Field, FString FieldName, TSharedPtr<FJsonObject> Result);

	UFUNCTION(BlueprintCallable)
		static void WriteStringToFile(FString Path, FString resultString, bool Relative);
	UFUNCTION(BlueprintCallable)
		static UTexture2D* LoadTextureFromFile(FString& String, FString Path, bool Relative);
	UFUNCTION(BlueprintCallable)
		static bool LoadStringFromFile(FString& String, FString Path, bool Relative);
	UFUNCTION(BlueprintCallable)
		static bool GetDirectoriesInPath(const FString& FullPathOfBaseDir, TArray<FString>& DirsOut, const FString& NotContainsStr, bool Recursive, const FString& ContainsStr);
	UFUNCTION(BlueprintCallable)
		static void String_Sort(UPARAM(ref) TArray <FString>& Array_To_Sort, bool Descending, bool FilterToUnique, TArray <FString>& Sorted_Array);
	UFUNCTION(BlueprintCallable)
		static bool GetFilesInPath(const FString& FullPathOfBaseDir, TArray<FString>& FilenamesOut, bool Recursive, const FString& FilterByExtension);
	UFUNCTION(BlueprintCallable)
		static TSubclassOf<UObject> FindClassWithLog(FString Name, UClass* ParentClass, UContentLibSubsystem * RootFolder);
	UFUNCTION(BlueprintCallable)
		static TSubclassOf<UObject> CreateContentLibClass(FString Name, UClass* ParentClass);

	UFUNCTION(BlueprintCallable)
		static bool StringCompareItem(FString e, FString Name, FString Prefix, FString Suffix);
	UFUNCTION(BlueprintCallable)
		static UClass* SetCategoryWithLoad(const FString CategoryString,
		                                UContentLibSubsystem* Subsystem, bool Schematic);
	UFUNCTION(BlueprintCallable)
		static void AddToItemAmountArray(TArray<FItemAmount> & Array,TMap<FString,int32> Cost,TArray<UClass*> Items, bool ClearFirst);

	UFUNCTION(BlueprintCallable)
		static void AddRecipeToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem,
	                       TSubclassOf<UFGRecipe> Recipe);

	UFUNCTION(BlueprintCallable)
	static void AddSchematicToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem,
	                          TSubclassOf<UFGSchematic> SchematicToAdd);

	UFUNCTION(BlueprintCallable)
		static void AddInfoOnlyToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, 
			FContentLib_UnlockInfoOnly InfoOnlyToAdd);

	UFUNCTION(BlueprintCallable)
	static void AddInventorySlotsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, int32 Slots);

	UFUNCTION(BlueprintCallable)
	static void AddGiveItemsToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const TMap<FString,int32> ItemsToGive, bool ClearFirst);

	UFUNCTION(BlueprintCallable)
	static void AddArmSlotsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, int32 Slots);


	UFUNCTION(BlueprintCallable)
	static void UnlockUnlockedRecipes(UContentLibSubsystem* Subsystem);

	UFUNCTION(BlueprintCallable)
	static UFGSchematicPurchasedDependency* FindFirstOrCreateSchematicPurchasedDependencyObj(TSubclassOf<UFGSchematic> Schematic);

	UFUNCTION(BlueprintCallable)
	static void AddSchematicToPurchaseDep(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem,
	                               TSubclassOf<UFGSchematic> SchematicDep);


	UFUNCTION(BlueprintPure, Category = "Class", meta = (DisplayName = "To CDO", CompactNodeTitle = "CDO", BlueprintAutocast))
    static UObject* Conv_ClassToObject(UClass* Class);

	UFUNCTION(BlueprintCallable)
    static UTexture2D* GetIconForBuilding(UContentLibSubsystem* System, TSubclassOf<AFGBuildable> Buildable, bool Big, const UObject* WorldContext);

	UFUNCTION(BlueprintPure)
    static TArray<FString> GetBlueprintFunctionNames(UClass * BlueprintClass);

	// Returns true and logs error messages if passed string doesn't pass some basic json validation logic
	UFUNCTION(BlueprintCallable)
	static bool FailsBasicJsonFormCheck(FString jsonString);

	/**
	  * DANGEROUS!
	  * If the object has a function of the given name with any signature, calls it with no arguments and returns true.
	  * When called from Blueprint, this function will log a warning when called citing your mod for safety reasons.
	  */
	UFUNCTION(BlueprintCallable, Category = "Reflection|ContentLib", CustomThunk)
	static bool CL_ExecuteArbitraryFunction(FName functionName, UObject* onObject);

	DECLARE_FUNCTION(execCL_ExecuteArbitraryFunction) {
		P_GET_PROPERTY(FNameProperty, functionName);
		P_GET_OBJECT(UObject, onObject);
		P_FINISH;

		P_NATIVE_BEGIN;
		UPackage* OutermostPackage = Stack.Node->GetOutermost();
		const FString PackageOwner = UBlueprintAssetHelperLibrary::FindPluginNameByObjectPath(OutermostPackage->GetName());

		UE_LOG(LogContentLib, Warning, TEXT("CL_ExecuteArbitraryFunction Mod '%s' is trying to call function '%s' on object '%s', this could cause a crash if used incorrectly"), *PackageOwner, *functionName.ToString(), *UKismetSystemLibrary::GetDisplayName(onObject));

		*(bool*)RESULT_PARAM = CL_ExecuteArbitraryFunction(functionName, onObject);
		P_NATIVE_END;
	}

	/**
	  * Gets the name of the mod currently doing stuff in the call stack. Blueprint only.
	  */
	UFUNCTION(BlueprintPure, Category = "Reflection|ContentLib", CustomThunk)
	static FString CL_GetExecutingPackage();

	DECLARE_FUNCTION(execCL_GetExecutingPackage) {
		P_FINISH;

		P_NATIVE_BEGIN;
		UPackage* OutermostPackage = Stack.Node->GetOutermost();
		const FString PackageOwner = UBlueprintAssetHelperLibrary::FindPluginNameByObjectPath(OutermostPackage->GetName());
		*(FString*)RESULT_PARAM = PackageOwner;
		P_NATIVE_END;
	}
};
