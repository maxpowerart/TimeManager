using UnrealBuildTool;

public class TimeManagerEditor : ModuleRules
{
	public TimeManagerEditor(ReadOnlyTargetRules Target) : base(Target)
	{

		PrivateIncludePaths.AddRange(
			new string[]
			{
				"TimeManager/Public",
				"TimeManager/Private",
				"TimeManagerEditor/Public",
				"TimeManagerEditor/Private"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"TimeManager"
			});

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"PropertyEditor",
				"Slate",
				"SlateCore",
				"EditorStyle"
			});
	}
}