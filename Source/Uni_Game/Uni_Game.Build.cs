// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Uni_Game : ModuleRules
{
	public Uni_Game(ReadOnlyTargetRules Target) : base(Target)
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
			"Uni_Game",
			"Uni_Game/Variant_Platforming",
			"Uni_Game/Variant_Platforming/Animation",
			"Uni_Game/Variant_Combat",
			"Uni_Game/Variant_Combat/AI",
			"Uni_Game/Variant_Combat/Animation",
			"Uni_Game/Variant_Combat/Gameplay",
			"Uni_Game/Variant_Combat/Interfaces",
			"Uni_Game/Variant_Combat/UI",
			"Uni_Game/Variant_SideScrolling",
			"Uni_Game/Variant_SideScrolling/AI",
			"Uni_Game/Variant_SideScrolling/Gameplay",
			"Uni_Game/Variant_SideScrolling/Interfaces",
			"Uni_Game/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
