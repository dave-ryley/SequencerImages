using UnrealBuildTool;

public class SequencerImagesEditor : ModuleRules
{
    public SequencerImagesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Sequencer",
                "SequencerImages", 
                "MovieScene",
                "MovieSceneTools",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "Sequencer",
                "SequencerImages",
                "MovieScene",
                "LevelSequence",
                "ImageCore"
            }
        );
    }
}