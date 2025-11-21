// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Inoutbox : ModuleRules
{
	public Inoutbox(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore" });
		
		PublicIncludePaths.AddRange(
			new string[]
			{
				ModuleDirectory
			});

		PrivateIncludePaths.AddRange(
			new string[]
			{
				ModuleDirectory
			});
	}
}
