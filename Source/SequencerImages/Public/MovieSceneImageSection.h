// Copyright © 2025 Dave Ryley

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSection.h"
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
	void EnsureBrushHasTexture();

private:
	virtual void ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity) override;

	UPROPERTY()
	UTexture2D* Texture;

	FSlateBrush Brush;
};
