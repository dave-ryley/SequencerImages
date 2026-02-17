// Copyright © 2026 Dave Ryley

#include "SequencerImageTrackInstance.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "SLevelViewport.h"
#endif
#include "MovieSceneImageSection.h"
#include "SequencerImages.h"

void USequencerImageTrackInstance::OnBeginUpdateInputs()
{
	
}

void USequencerImageTrackInstance::OnInputAdded(const FMovieSceneTrackInstanceInput& InInput)
{
	if (UMovieSceneImageSection* ImageSection = Cast<UMovieSceneImageSection>(InInput.Section))
	{
		Sections.Add(ImageSection);
		
		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LogSequencerImages, Log, TEXT("No World Found"));
			return;
		}
		
		ImageSection->EnsureBrushHasTexture();
		const FSlateBrush* Brush = ImageSection->GetBrush();
		TSharedPtr<SBorder> Border = SNew(SBorder)
		.BorderImage( FAppStyle::GetBrush( TEXT("WhiteTexture") ) )
		.Padding(FMargin{0})
		.BorderBackgroundColor(ImageSection->GetBackgroundColor())
		[
			SNew(SScaleBox)
			.Stretch(ImageSection->GetLockAspectRatio() ? EStretch::ScaleToFit : EStretch::Fill)
			[
				SNew(SImage)
				.Image(Brush)
				.RenderOpacity(ImageSection->GetUsingOnionSkin() ? ImageSection->GetOnionSkinOpacity() : 1.0f)
			]
		];

		if (World->IsGameWorld())
		{
			UGameViewportClient* ViewportClient = World->GetGameViewport();
			if (!ViewportClient)
			{
				UE_LOG(LogSequencerImages, Log, TEXT("No ViewportClient Found"));
				return;
			}
			ViewportClient->AddViewportWidgetContent(Border.ToSharedRef());
		}
#if WITH_EDITOR
		else
		{
			FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>("LevelEditor");
			TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();
			if (LevelEditor.IsValid())
			{
				for (TSharedPtr<SLevelViewport> LevelViewport : LevelEditor->GetViewports())
				{
					LevelViewport->AddOverlayWidget(Border.ToSharedRef());
				}
			}
		}
#endif

		ImageMap.Add(ImageSection, MoveTemp(Border));
	}
}

void USequencerImageTrackInstance::OnAnimate()
{
	// for (auto SectionWidgetPair : ImageMap)
	// {
	// 	SectionWidgetPair.Value->SetColorAndOpacity(FSlateColor{0,0,0,SectionWidgetPair.Key.GetOpacity})
	// }
}

void USequencerImageTrackInstance::OnInputRemoved(const FMovieSceneTrackInstanceInput& InInput)
{
	if (UMovieSceneImageSection* ImageSection = Cast<UMovieSceneImageSection>(InInput.Section))
	{
		Sections.Remove(ImageSection);

		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LogSequencerImages, Warning, TEXT("No World Found"));
			return;
		}

		if (ImageMap.Contains(ImageSection))
		{
			if (TSharedPtr<SBorder> BorderWidget = ImageMap[ImageSection]; BorderWidget.IsValid())
			{
				if (World->IsGameWorld())
				{
					UGameViewportClient* ViewportClient = World->GetGameViewport();
					if (!ViewportClient)
					{
						UE_LOG(LogSequencerImages, Warning, TEXT("No ViewportClient Found"));
						return;
					}
					ViewportClient->RemoveViewportWidgetContent(BorderWidget.ToSharedRef());
				}
#if WITH_EDITOR
				else
				{
					FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>("LevelEditor");
					TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();
					if (LevelEditor.IsValid())
					{
						for (TSharedPtr<SLevelViewport> LevelViewport : LevelEditor->GetViewports())
						{
							LevelViewport->RemoveOverlayWidget(BorderWidget.ToSharedRef());
						}
					}
				}
#endif
			}
			
		}
	}
}

void USequencerImageTrackInstance::OnDestroyed()
{
	UE_LOG(LogSequencerImages, Log, TEXT("OnDestroyed"));
	Sections.Empty();

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSequencerImages, Warning, TEXT("No World Found"));
		return;
	}

	if (World->IsGameWorld())
	{
		UGameViewportClient* ViewportClient = World->GetGameViewport();
		if (!ViewportClient)
		{
			UE_LOG(LogSequencerImages, Warning, TEXT("No ViewportClient Found"));
			return;
		}
		for (TTuple ImagePair : ImageMap)
		{
			if (ImagePair.Value.IsValid())
			{
				ViewportClient->RemoveViewportWidgetContent(ImagePair.Value.ToSharedRef());
			}
		}
	}
#if WITH_EDITOR
	else
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::Get().GetModuleChecked<FLevelEditorModule>("LevelEditor");
		TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();
		if (LevelEditor.IsValid())
		{
			for (TSharedPtr<SLevelViewport> LevelViewport : LevelEditor->GetViewports())
			{
				for (TTuple ImagePair : ImageMap)
				{
					if (ImagePair.Value.IsValid())
					{
						LevelViewport->RemoveOverlayWidget(ImagePair.Value.ToSharedRef());
					}
				}
			}
		}
	}
#endif
}
