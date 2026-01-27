#pragma once

#include "CoreMinimal.h"
#include "ISequencerModule.h"
#include "Modules/ModuleManager.h"

class FSequencerImagesEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    FDelegateHandle SequencerImageTrackCreateEditorHandle;
};
