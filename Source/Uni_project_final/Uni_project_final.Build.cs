// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Uni_project_final : ModuleRules
{
	public Uni_project_final(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Uni_project_final",
			"Uni_project_final/Variant_Platforming",
			"Uni_project_final/Variant_Platforming/Animation",
			"Uni_project_final/Variant_Combat",
			"Uni_project_final/Variant_Combat/AI",
			"Uni_project_final/Variant_Combat/Animation",
			"Uni_project_final/Variant_Combat/Gameplay",
			"Uni_project_final/Variant_Combat/Interfaces",
			"Uni_project_final/Variant_Combat/UI",
			"Uni_project_final/Variant_SideScrolling",
			"Uni_project_final/Variant_SideScrolling/AI",
			"Uni_project_final/Variant_SideScrolling/Gameplay",
			"Uni_project_final/Variant_SideScrolling/Interfaces",
			"Uni_project_final/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
