// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TimeManager : ModuleRules
{
	public TimeManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		MinFilesUsingPrecompiledHeaderOverride = 1;
		ShadowVariableWarningLevel = WarningLevel.Warning;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}
