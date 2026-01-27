#include "SequencerImagesEditor.h"

#include "ISequencerModule.h"
#include "TrackEditors/SequencerImageTrackEditor.h"

#define LOCTEXT_NAMESPACE "FSequencerImagesEditorModule"

void FSequencerImagesEditorModule::StartupModule()
{
	using namespace UE::Sequencer;

	if (GIsEditor)
	{
		ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
		SequencerImageTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FSequencerImageTrackEditor::CreateTrackEditor ) );
	}
}

void FSequencerImagesEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("Sequencer"))
	{
		return;
	}
	
	ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>( "Sequencer" );
	SequencerModule.UnRegisterTrackEditor( SequencerImageTrackCreateEditorHandle );
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSequencerImagesEditorModule, SequencerImagesEditor)