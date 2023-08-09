// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Runtime.InteropServices;
using System.Text;
using EpicGames.Core;

public class ContentLib : ModuleRules
{
	public ContentLib(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		var factoryGamePchPath = new DirectoryReference(Path.Combine(Target.ProjectFile.Directory.ToString(), "Source", "FactoryGame", "Public", "FactoryGame.h"));
		PrivatePCHHeaderFile = factoryGamePchPath.MakeRelativeTo(new DirectoryReference(ModuleDirectory));

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject",
				"Engine",
				"InputCore","UMG","SlateCore","ImageWrapper", "RenderCore",
				"FactoryGame", "SML", "Json"
			}
			);
	}
}
