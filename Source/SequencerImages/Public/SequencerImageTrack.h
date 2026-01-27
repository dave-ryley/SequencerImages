// Copyright © 2025 Dave Ryley

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneNameableTrack.h"
#include "SequencerImageTrack.generated.h"

/**
 * Handles Images displayed on screen for storyboards, title cards, or other uses.
 */
UCLASS()
class SEQUENCERIMAGES_API USequencerImageTrack : public UMovieSceneNameableTrack
{
	GENERATED_BODY()

	USequencerImageTrack(const FObjectInitializer& ObjectInitializer);

public:
	virtual bool SupportsMultipleRows() const override { return false; }
	virtual EMovieSceneTrackEasingSupportFlags SupportsEasing(FMovieSceneSupportsEasingParams& Params) const override { return EMovieSceneTrackEasingSupportFlags::None; }
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual UMovieSceneSection* CreateNewSection() override;

	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override { return Sections; }
	virtual void AddSection(UMovieSceneSection& Section) override { Sections.AddUnique(&Section); }
	virtual void RemoveSection(UMovieSceneSection& Section) override { Sections.Remove(&Section); }
	virtual void RemoveSectionAt(int32 SectionIndex) override { Sections.RemoveAt(SectionIndex); }
	virtual bool HasSection(const UMovieSceneSection& Section) const override { return Sections.Contains(&Section); }
	virtual bool IsEmpty() const override { return Sections.Num() == 0; }

	virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
	
	UMovieSceneSection* AddNewImageOnRow(UTexture2D* Texture, FFrameNumber FrameNumber, int RowIndex);

protected:
	/** All the sections in this list */
	UPROPERTY()
	TArray<TObjectPtr<UMovieSceneSection>> Sections;

#if WITH_EDITORONLY_DATA

public:

	/**
	 * Get the height of this track's rows
	 */
	int32 GetRowHeight() const
	{
		return RowHeight;
	}

	/**
	 * Set the height of this track's rows
	 */
	void SetRowHeight(int32 NewRowHeight)
	{
		RowHeight = FMath::Max(16, NewRowHeight);
	}

private:

	/** The height for each row of this track */
	UPROPERTY()
	int32 RowHeight;

#endif

};
