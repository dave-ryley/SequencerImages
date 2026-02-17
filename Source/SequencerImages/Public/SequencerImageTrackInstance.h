// Copyright © 2026 Dave Ryley

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneImageSection.h"
#include "EntitySystem/TrackInstance/MovieSceneTrackInstance.h"
#include "SequencerImageTrackInstance.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCERIMAGES_API USequencerImageTrackInstance : public UMovieSceneTrackInstance
{
	GENERATED_BODY()

private:
	virtual void OnBeginUpdateInputs() override;
	
	virtual void OnInputAdded(const FMovieSceneTrackInstanceInput& InInput) override;

	virtual void OnAnimate() override;

	virtual void OnInputRemoved(const FMovieSceneTrackInstanceInput& InInput) override;
	
	virtual void OnDestroyed() override;

private:
	UPROPERTY()
	TArray<UMovieSceneImageSection*> Sections;

	TMap<UMovieSceneImageSection*, TSharedPtr<SBorder>> ImageMap;
};
