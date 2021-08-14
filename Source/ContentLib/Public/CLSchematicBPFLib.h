

#pragma once

#include "CoreMinimal.h"



#include "CLItemBPFLib.h"
#include "FGSchematic.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CLSchematicBPFLib.generated.h"

class UContentLibSubsystem;


USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_Vector2D
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		int32 X =0;	
	UPROPERTY(BlueprintReadWrite)
		int32 Y =0;
};


USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_ResearchNodeRoads
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FContentLib_Vector2D ChildNode;
	UPROPERTY(BlueprintReadWrite)
	TArray<FContentLib_Vector2D> Roads;
};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_ResearchNode
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString ResearchTree;

	UPROPERTY(BlueprintReadWrite)
	FContentLib_Vector2D Coordinates;

	UPROPERTY(BlueprintReadWrite)
	TArray<FContentLib_Vector2D> Parents;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FContentLib_Vector2D> UnHiddenBy;

	UPROPERTY(BlueprintReadWrite)
	TArray<FContentLib_Vector2D> NodesToUnHide;

	UPROPERTY(BlueprintReadWrite)
	TArray<FContentLib_ResearchNodeRoads> Children;

};

USTRUCT(BlueprintType)
struct  CONTENTLIB_API  FContentLib_Schematic
{
	GENERATED_BODY()
	FContentLib_Schematic(): MenuPriority(-1), Tier(-1), Time(-1), SlotsToGive(-1), ArmSlotsToGive(-1), ClearCats(false),
	                         ClearCost(false),
	                         ClearRecipes(false),
	                         ClearDeps(false), ClearItemsToGive(false)
	{
	}
	;

	
	UPROPERTY(BlueprintReadWrite)
	FString Type;
	UPROPERTY(BlueprintReadWrite)
	FString Name;
	UPROPERTY(BlueprintReadWrite)
	FString Description;
	UPROPERTY(BlueprintReadWrite)
	FString Cat;
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> SubCat;
	UPROPERTY(BlueprintReadWrite)
	float MenuPriority;
	UPROPERTY(BlueprintReadWrite)
	int32 Tier;
	UPROPERTY(BlueprintReadWrite)
	TMap< FString,int32 > Cost;
	UPROPERTY(BlueprintReadWrite)
	float Time;
	UPROPERTY(BlueprintReadWrite)
	TArray< FString > Recipes;
	UPROPERTY(BlueprintReadWrite)
	TArray< FString > Schematics;
	UPROPERTY(BlueprintReadWrite)
	int32 SlotsToGive;
	UPROPERTY(BlueprintReadWrite)
	int32 ArmSlotsToGive;
	UPROPERTY(BlueprintReadWrite)
	TMap< FString,int32 > ItemsToGive;
	UPROPERTY(BlueprintReadWrite)
	FString VisualKit;
	UPROPERTY(BlueprintReadWrite)
	TArray< FString > DependsOn;
	UPROPERTY(BlueprintReadWrite)
	FContentLib_ResearchNode ResearchTree;
	
	UPROPERTY(BlueprintReadWrite)
	bool ClearCats;
	UPROPERTY(BlueprintReadWrite)
	bool ClearCost;
	UPROPERTY(BlueprintReadWrite)
	bool ClearRecipes;
	UPROPERTY(BlueprintReadWrite)
	bool ClearDeps;
	UPROPERTY(BlueprintReadWrite)
	bool ClearItemsToGive;
};


UCLASS()
class CONTENTLIB_API UCLSchematicBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	UFUNCTION(BlueprintCallable)
	static FContentLib_ResearchNode GenerateResearchStructFromString(FString String);
	UFUNCTION(BlueprintCallable)
	static FContentLib_ResearchNodeRoads GenerateResearchNodeRoadsFromString(FString String);
	UFUNCTION(BlueprintCallable)
	static FContentLib_Vector2D GenerateVector2DFromString(FString String);
	UFUNCTION(BlueprintCallable)
    static FContentLib_Schematic GenerateCLSchematicFromString(FString String);
	UFUNCTION(BlueprintCallable)
	static void InitSchematicFromStruct(FContentLib_Schematic Schematic, TSubclassOf<UFGSchematic> SchematicClass, UContentLibSubsystem* SubSystem);

	UFUNCTION(BlueprintCallable)
    static FString SerializeSchematic(TSubclassOf<UFGSchematic> Schematic);
	UFUNCTION(BlueprintCallable)
    static FString SerializeCLSchematic(FContentLib_Schematic Schematic);

	UFUNCTION(BlueprintCallable)
    static void ApplyVisualKitToSchematic(UContentLibSubsystem* Subsystem, FContentLib_VisualKit Kit, TSubclassOf<UFGSchematic> Item);
};
