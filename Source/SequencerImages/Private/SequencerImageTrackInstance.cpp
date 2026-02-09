// Copyright © 2026 Dave Ryley


#include "SequencerImageTrackInstance.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "SLevelViewport.h"
#endif
#include "MovieSceneImageSection.h"
#include "SequencerImages.h"

void USequencerImageTrackInstance::OnInputAdded(const FMovieSceneTrackInstanceInput& InInput)
{
	if (UMovieSceneImageSection* ImageSection = Cast<UMovieSceneImageSection>(InInput.Section))
	{
		Sections.Add(ImageSection);

		ImageSection->EnsureBrushHasTexture();
		const FSlateBrush* Brush = ImageSection->GetBrush();

		TSharedPtr<SImage> ImageWidget = SNew(SImage)
			.Image(Brush);
		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LogSequencerImages, Log, TEXT("No World Found"));
			return;
		}

		if (World->IsGameWorld())
		{
			UGameViewportClient* ViewportClient = World->GetGameViewport();
			if (!ViewportClient)
			{
				UE_LOG(LogSequencerImages, Log, TEXT("No ViewportClient Found"));
				return;
			}
			ViewportClient->AddViewportWidgetContent(ImageWidget.ToSharedRef());
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
					LevelViewport->AddOverlayWidget(ImageWidget.ToSharedRef());
				}
			}
		}
#endif

		ImageMap.Add(ImageSection, MoveTemp(ImageWidget));
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
			if (TSharedPtr<SImage> ImageWidget = ImageMap[ImageSection]; ImageWidget.IsValid())
			{
				if (World->IsGameWorld())
				{
					UGameViewportClient* ViewportClient = World->GetGameViewport();
					if (!ViewportClient)
					{
						UE_LOG(LogSequencerImages, Warning, TEXT("No ViewportClient Found"));
						return;
					}
					ViewportClient->RemoveViewportWidgetContent(ImageWidget.ToSharedRef());
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
							LevelViewport->RemoveOverlayWidget(ImageWidget.ToSharedRef());
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
		for (TTuple<UMovieSceneImageSection*, TSharedPtr<SImage>> ImagePair : ImageMap)
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
				for (TTuple<UMovieSceneImageSection*, TSharedPtr<SImage>> ImagePair : ImageMap)
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
