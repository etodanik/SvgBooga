using System.IO;
using UnrealBuildTool;

public class LunaSvg : ModuleRules
{
	public LunaSvg(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PublicSystemIncludePaths.Add("$(ModuleDir)/Public");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the static library for Windows
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "lunasvg.lib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			// Add the static library for Mac
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "liblunasvg.a"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			// Add the static library for Linux
			var lunasvgSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "LunaSvg", "Linux",
				"x86_64-unknown-linux-gnu", "liblunasvg.a");
			PublicAdditionalLibraries.Add(lunasvgSoPath);
		}
	}
}