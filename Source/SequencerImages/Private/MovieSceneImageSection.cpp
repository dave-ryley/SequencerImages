// Copyright © 2026 Dave Ryley

#include "MovieSceneImageSection.h"

#include "SequencerImageTrackInstance.h"
#include "EntitySystem/BuiltInComponentTypes.h"

UMovieSceneImageSection::UMovieSceneImageSection(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
	OpacityMultiplier.SetDefault(1.f);
}

void UMovieSceneImageSection::SetTexture(UTexture2D* InTexture)
{
	if (IsValid(Texture))
	{
		Texture->RemoveFromRoot();
	}
	Texture = InTexture;
	Texture->AddToRoot();
	if (Texture)
	{
		Texture->bForceMiplevelsToBeResident = true;
		Texture->bIgnoreStreamingMipBias = true;
	}
	EnsureBrushHasTexture();
}

float UMovieSceneImageSection::GetOpacityAtFrame(const FFrameTime FrameTime) const
{
	float Result = 1.f;
	OpacityMultiplier.Evaluate(FrameTime, Result);
	return Result;
}

void UMovieSceneImageSection::EnsureBrushHasTexture()
{
	if(Brush.GetResourceObject() != Texture)
	{
		Brush.SetResourceObject(Texture);
	}
}

void UMovieSceneImageSection::InitialPlacementOnRow(const TArray<UMovieSceneSection*>& Sections,
	FFrameNumber InStartTime, int32 Duration, int32 InRowIndex)
{
	check(Duration >= 0);
	// We don't necessarily want to get the furthest end frame if we can slot the new section between existing sections.
	// Most straightforward approach is to keep moving section to the end of any section we overlap until we don't overlap any
	bool bFoundOverlap = true;
	while (bFoundOverlap)
	{
		TRange<FFrameNumber> ThisRange = TRange<FFrameNumber>(InStartTime, InStartTime + Duration);
		bFoundOverlap = false;
		for (const auto Section : Sections)
		{
			check(Section);
			if ((this != Section) && (Section->GetRowIndex() == InRowIndex))
			{
				if (ThisRange.Overlaps(Section->GetRange()))
				{
					InStartTime = Section->GetExclusiveEndFrame();
					bFoundOverlap = true;
					break;
				}
			}
		}
	}
	Super::InitialPlacementOnRow(Sections, InStartTime, Duration, InRowIndex);
}

void UMovieSceneImageSection::ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker,
                                               const FEntityImportParams& Params, FImportedEntity* OutImportedEntity)
{
	using namespace UE::MovieScene;
	
	FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();

	FGuid ObjectBindingID = Params.GetObjectBindingID();
	OutImportedEntity->AddBuilder(
		FEntityBuilder()
		.Add(BuiltInComponents->TrackInstance, FMovieSceneTrackInstanceComponent{ decltype(FMovieSceneTrackInstanceComponent::Owner)(this), USequencerImageTrackInstance::StaticClass() })
		.AddConditional(BuiltInComponents->GenericObjectBinding, ObjectBindingID, ObjectBindingID.IsValid())
		.AddTagConditional(BuiltInComponents->Tags.Root, !ObjectBindingID.IsValid())
		.Add(BuiltInComponents->FloatChannel[0], &OpacityMultiplier)
	);
}
