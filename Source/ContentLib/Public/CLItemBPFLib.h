

#pragma once

#include "CoreMinimal.h"

#include "FGSchematic.h"
#include "Dom/JsonObject.h"
#include "Resources/FGItemDescriptorNuclearFuel.h"
#include "Resources/FGResourceDescriptor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CLItemBPFLib.generated.h"



class UContentLibSubsystem;

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_VisualKit
{
	GENERATED_BODY()

	
	FContentLib_VisualKit();
	
	UTexture2D* GetSmallIcon() const;
	UTexture2D* GetBigIcon() const;
	class UStaticMesh* GetMesh() const;
	
	static TSharedRef<FJsonObject> GetAsJsonObject(TSubclassOf<UFGItemDescriptor> Item);


	UPROPERTY(BlueprintReadWrite)
	FString Mesh;
	
	UPROPERTY(BlueprintReadWrite)
	FString BigIcon;
	
	UPROPERTY(BlueprintReadWrite)
	FString SmallIcon;
	
	UPROPERTY(BlueprintReadWrite)
	FColor FluidColor;
	
	UPROPERTY(BlueprintReadWrite)
	FColor GasColor;

};
USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_ResourceItem
{
	GENERATED_BODY()
	FContentLib_ResourceItem();
	
	static TSharedRef<FJsonObject> GetResourceAsJsonObject(TSubclassOf<UFGResourceDescriptor> Item);

	UPROPERTY(BlueprintReadWrite)
	FLinearColor PingColor;

	UPROPERTY(BlueprintReadWrite)
	float CollectSpeedMultiplier;
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_NuclearFuelItem
{
	GENERATED_BODY()
	FContentLib_NuclearFuelItem();

	static TSharedRef<FJsonObject> GetNuclearFuelAsJsonObject(TSubclassOf<UFGItemDescriptorNuclearFuel> Item);

	
	UPROPERTY(BlueprintReadWrite)
	FString SpentFuelClass;

	UPROPERTY(BlueprintReadWrite)
	int32 AmountOfWaste;
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_Item
{
	GENERATED_BODY()


	FContentLib_Item();
	friend class UContentLib_ItemSubsystem;
	
	UPROPERTY(BlueprintReadWrite)
	EResourceForm Form ;
	UPROPERTY(BlueprintReadWrite)
	EStackSize StackSize ;
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString NameShort;
	UPROPERTY(BlueprintReadWrite)
	FString Description;
	UPROPERTY(BlueprintReadWrite)
	FString Category;
	UPROPERTY(BlueprintReadWrite)
	FString VisualKit;
	
	UPROPERTY(BlueprintReadWrite)
	float EnergyValue;
	UPROPERTY(BlueprintReadWrite)
	float RadioactiveDecay;

	UPROPERTY(BlueprintReadWrite)
	int32 CanBeDiscarded;
	UPROPERTY(BlueprintReadWrite)
	int32 RememberPickUp;

	
	UPROPERTY(BlueprintReadWrite)
	int32 ResourceSinkPoints;

	UPROPERTY(BlueprintReadWrite)
	FContentLib_ResourceItem ResourceItem;

	UPROPERTY(BlueprintReadWrite)
	FContentLib_NuclearFuelItem FuelWasteItem;
	
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_ItemPatch
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UFGItemDescriptor> Class;
	UPROPERTY(BlueprintReadWrite)
	FString Json;
};

/**
 * 
 */
UCLASS()
class CONTENTLIB_API UCLItemBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:
	
	UFUNCTION(BlueprintCallable)
    static FString GenerateFromDescriptorClass(TSubclassOf<UFGItemDescriptor> Item);
    UFUNCTION(BlueprintCallable)
	static FContentLib_Item GenerateCLItemFromString(FString String);

	UFUNCTION(BlueprintCallable)
	static 	FString GenerateStringFromCLItem(FContentLib_Item Item);
    
    UFUNCTION(BlueprintCallable)
    static FContentLib_VisualKit GenerateKitFromString(FString String);

	UFUNCTION(BlueprintCallable)
	static FString GenerateKitFromClass(TSubclassOf<UFGItemDescriptor> Item);

   
    UFUNCTION(BlueprintCallable)
    static FString GetVisualKitAsString(FContentLib_VisualKit Kit);
    
    UFUNCTION(BlueprintCallable)
    static void InitItemFromStruct(const TSubclassOf<UFGItemDescriptor> Item, FContentLib_Item Struct, UContentLibSubsystem* Subsystem);


	UFUNCTION(BlueprintCallable)
	static FString GenerateFromNuclearFuelClass(TSubclassOf<UFGItemDescriptor> Item);


	UFUNCTION(BlueprintCallable)
	static FString GenerateResourceFromClass(TSubclassOf<UFGItemDescriptor> Item);

	UFUNCTION(BlueprintCallable)
	static void ApplyVisualKitToItem(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit,TSubclassOf<UFGItemDescriptor> Item);
		
};
