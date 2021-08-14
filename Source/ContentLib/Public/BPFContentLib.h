

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ContentLibSubsystem.h"
#include "FGItemCategory.h"
#include "FGRecipe.h"
#include "FGSchematic.h"
#include "ItemAmount.h"
#include "Dom/JsonObject.h"
#include "Serialization/ObjectWriter.h"
#include "Serialization/ObjectReader.h" 
#include "BPFContentLib.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FObjectFunctionBind, UObject* , Object);

template<typename Tag, typename Tag::type M>
struct Grab {
	friend typename Tag::type get(Tag) {
		return M;
	}
};

struct grab_FuncTable {
	typedef TMap<FName, UFunction*> UClass::* type;
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
	static void AddSlotsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, int32 Slots);

	UFUNCTION(BlueprintCallable)
	static void AddGiveItemsToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLibSubsystem* Subsystem, const TMap<FString,int32> ItemsToGive, bool ClearFirst);

	UFUNCTION(BlueprintCallable)
	static void AddArmSlotsToUnlock(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem, int32 Slots);


	UFUNCTION(BlueprintCallable)
	static void UnlockUnlockedRecipes(UContentLibSubsystem* Subsystem);


	UFUNCTION(BlueprintCallable)
	static void AddSchematicToPurchaseDep(TSubclassOf<UFGSchematic> Schematic, UContentLibSubsystem* Subsystem,
	                               TSubclassOf<UFGSchematic> SchematicDep);


	UFUNCTION(BlueprintPure, Category = "Class", meta = (DisplayName = "To CDO", CompactNodeTitle = "CDO", BlueprintAutocast))
    static UObject* Conv_ClassToObject(UClass* Class);

	UFUNCTION(BlueprintCallable)
    static void BindOnBPFunction(TSubclassOf<UObject> Class, FObjectFunctionBind Binding, FString FunctionName);

	UFUNCTION(BlueprintCallable)
    static UTexture2D* GetIconForBuilding(UContentLibSubsystem* System, TSubclassOf<AFGBuildable> Buildable, bool Big, const UObject* WorldContext);

	UFUNCTION(BlueprintPure)
    static TArray<FString> GetBlueprintFunctionNames(UClass * BlueprintClass);


	static FObjectFunctionBind WidgetBind;
};
