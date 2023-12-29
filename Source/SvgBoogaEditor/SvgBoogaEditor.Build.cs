using UnrealBuildTool;

public class SvgBoogaEditor : ModuleRules
{
	public SvgBoogaEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core", "AssetTools"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"LunaSvg",
				"Projects",
				"Engine",
				"Renderer",
				"InputCore",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"RenderCore",
				"SvgBooga"
			}
		);
	}
}