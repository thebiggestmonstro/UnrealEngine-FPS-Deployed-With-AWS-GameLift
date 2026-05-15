using UnrealBuildTool;

public class DedicatedServers : ModuleRules
{
	public DedicatedServers(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "Core", 
            "CoreUObject", 
            "Engine",
            "GameplayTags",
        });

        if (Target.Type == TargetType.Server || Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.Add("GameLiftServerSDK");
            PublicDefinitions.Add("WITH_GAMELIFT=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMELIFT=0");
        }
        bEnableExceptions = true;

        PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"Slate", 
			"SlateCore",
            "UMG"
		});
	}
}
