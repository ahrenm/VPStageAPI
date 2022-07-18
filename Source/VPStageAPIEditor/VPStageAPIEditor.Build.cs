// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VPStageAPIEditor : ModuleRules
{
	public VPStageAPIEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);

	PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CinematicCamera",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorSubsystem", 
				"DisplayClusterConfiguration", "DisplayCluster", 
				"LevelSequence", "Sequencer"
				, "LevelSequenceEditor"
				,"UnrealEd","EditorFramework"
				,"MultiUserClient", "ConcertSyncClient","ConcertSyncCore", "Concert", "ConcertTransport", "ConcertTakeRecorder", "TakeRecorder", "TakesCore",
				/*
				"Projects",
				"EditorFramework",
				"InputCore",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"UMG",
				"Blutility",
				"EditorSubsystem",
				"Slate",
				"SlateCore",
				"AssetTools",
				"LevelSequence",
				"LevelSequenceEditor",
				"Sequencer",
				"Blutility",
				"UMGEditor",
				"AssetTools",
				"MultiUserClient",
				"ConcertSyncClient",
				"Concert",
				"ConcertTransport",
				"ConcertTakeRecorder",
				"TakeRecorder",
				"TakesCore",
				"DisplayCluster",
				"DisplayClusterConfiguration",
				"CinematicCamera",
				"Json",
				"JsonUtilities",
				"Networking",
				"Sockets",
				"SequencerWidgets",
				"CurveEditor",
				*/
				
				
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
