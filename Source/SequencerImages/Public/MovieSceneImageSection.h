// Copyright © 2026 Dave Ryley

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "EntitySystem/IMovieSceneEntityProvider.h"
#include "MovieSceneImageSection.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCERIMAGES_API UMovieSceneImageSection
	: public UMovieSceneSection
	, public IMovieSceneEntityProvider
{
	GENERATED_BODY()

	UMovieSceneImageSection(const FObjectInitializer& ObjInit);

public:
	void SetTexture(UTexture2D* InTexture);
	UTexture2D* GetTexture() const { return Texture; }
	const FSlateBrush* GetBrush() const { return &Brush; }
	float GetOpacityAtFrame(const FFrameTime FrameTime) const;
	void EnsureBrushHasTexture();
	/** Overriding InitialPlacementOnRow to ensure that multiple images dropped will cause sections to follow in order instead of create new rows. */
	virtual void InitialPlacementOnRow(const TArray<UMovieSceneSection*>& Sections, FFrameNumber InStartTime, int32 InDuration, int32 InRowIndex) override;

	bool GetLockAspectRatio() const { return bLockAspectRatio; }
	TAttribute<FSlateColor> GetBackgroundColor() const { return bOnionSkin ? FColor::Transparent : BackgroundColor; }
	float GetOnionSkinOpacity() const { return OnionSkinOpacity; }
	bool GetUsingOnionSkin() const { return bOnionSkin; }
	
private:
	virtual void ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity) override;

	UPROPERTY(EditAnywhere, Category="Section")
	bool bLockAspectRatio = true;

	UPROPERTY(EditAnywhere, Category="Section")
	bool bOnionSkin = false;

	UPROPERTY(EditAnywhere, Category="Section", meta=(EditCondition="bOnionSkin"))
	float OnionSkinOpacity = 0.25f;

	UPROPERTY(EditAnywhere, Category="Section")
	FSlateColor BackgroundColor = FColor::Black;
	
	UPROPERTY()
	UTexture2D* Texture;

	FSlateBrush Brush;

	UPROPERTY()
	FMovieSceneFloatChannel OpacityMultiplier;
};
