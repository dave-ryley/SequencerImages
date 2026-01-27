// Copyright © 2025 Dave Ryley

#include "SequencerImageTrack.h"
#include "MovieScene.h"
#include "MovieSceneImageSection.h"
#include "UObject/AssetRegistryTagsContext.h"

USequencerImageTrack::USequencerImageTrack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	TrackTint = FColor(180, 0, 40, 65);
	RowHeight = 50;
#endif
}

bool USequencerImageTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
	return SectionClass == UMovieSceneImageSection::StaticClass();
}

UMovieSceneSection* USequencerImageTrack::CreateNewSection()
{
	return NewObject<UMovieSceneImageSection>(this, NAME_None, RF_Transactional);
}

void USequencerImageTrack::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
	Super::GetAssetRegistryTags(Context);
}

UMovieSceneSection* USequencerImageTrack::AddNewImageOnRow(UTexture2D* Texture, FFrameNumber FrameNumber, int RowIndex)
{
	check(Texture);

	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();
	FFrameTime DurationToUse = 1.f * FrameRate;

	// add the section
	UMovieSceneImageSection* NewSection = Cast<UMovieSceneImageSection>(CreateNewSection());
	NewSection->InitialPlacementOnRow( Sections, FrameNumber, DurationToUse.FrameNumber.Value, RowIndex );
	NewSection->SetTexture(Texture);

	Sections.Add(NewSection);
	return NewSection;
}
