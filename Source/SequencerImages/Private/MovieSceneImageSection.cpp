// Copyright © 2025 Dave Ryley


#include "MovieSceneImageSection.h"

#include "SequencerImageTrackInstance.h"
#include "EntitySystem/BuiltInComponentTypes.h"

UMovieSceneImageSection::UMovieSceneImageSection(const FObjectInitializer& ObjInit)
	: Super(ObjInit)
{
}

void UMovieSceneImageSection::SetTexture(UTexture2D* InTexture)
{
	if (IsValid(Texture))
	{
		Texture->RemoveFromRoot();
	}
	Texture = InTexture;
	Texture->AddToRoot();
	if (Texture) // Since this texture is used as UI, don't allow it affected by budget.
	{
		Texture->bForceMiplevelsToBeResident = true;
		Texture->bIgnoreStreamingMipBias = true;
	}
	EnsureBrushHasTexture();
}

void UMovieSceneImageSection::EnsureBrushHasTexture()
{
	if(Brush.GetResourceObject() != Texture)
	{
		Brush.SetResourceObject(Texture);

		// 			if (bMatchSize)
		// 			{
		// 				if (Texture)
		// 				{
		// // #if WITH_EDITOR
		// // 					FTextureCompilingManager::Get().FinishCompilation({ Texture });
		// // #endif
		// 					Brush.ImageSize.X = static_cast<float>(Texture->GetSizeX());
		// 					Brush.ImageSize.Y = static_cast<float>(Texture->GetSizeY());
		// 				}
		// 				else
		// 				{
		// 					Brush.ImageSize = FVector2D(0, 0);
		// 				}
		// 			}

	}
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
	);
}
