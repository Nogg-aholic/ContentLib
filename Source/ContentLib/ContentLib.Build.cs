using UnrealBuildTool;
using System.IO;
using System;

public class ContentLib : ModuleRules
{
    public ContentLib(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // FactoryGame transitive dependencies
        // Not all of these are required, but including the extra ones saves you from having to add them later.
        PublicDependencyModuleNames.AddRange(new[] {
            "Core", "CoreUObject",
            "Engine",
            "DeveloperSettings",
            "PhysicsCore",
            "InputCore",
            "GeometryCollectionEngine",
            "ChaosVehiclesCore", "ChaosSolverEngine",
            "AnimGraphRuntime",
            "AssetRegistry",
            "NavigationSystem",
            "AIModule",
            "GameplayTasks",
            "SlateCore", "Slate", "UMG",
            "RenderCore",
            "CinematicCamera",
            "Foliage",
            "EnhancedInput",
            "NetCore",
            "GameplayTags",
		});

        // Header stubs
        PublicDependencyModuleNames.AddRange(new[] {
            "DummyHeaders",
        });

        // Required by ContentLib
        PublicDependencyModuleNames.AddRange(new[] {
            "Json",
        });

        if (Target.Type == TargetRules.TargetType.Editor) {
            PublicDependencyModuleNames.AddRange(new string[] {"AnimGraph"});
        }
        PublicDependencyModuleNames.AddRange(new string[] {"FactoryGame", "SML"});
    }
}