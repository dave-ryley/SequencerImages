#include "TrackEditors/SequencerImageTrackEditor.h"

#include "MovieSceneToolHelpers.h"
#include "SequencerImageTrack.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "LevelSequence.h"
#include "MovieSceneImageSection.h"
#include "SequencerSectionPainter.h"
#include "BaseGizmos/IntervalGizmo.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define LOCTEXT_NAMESPACE "SequencerImagesEditor"

FSequencerImageTrackEditor::FSequencerImageTrackEditor(TSharedRef<ISequencer> InSequencer)
	: FMovieSceneTrackEditor( InSequencer ) 
{
}

FSequencerImageTrackEditor::~FSequencerImageTrackEditor()
{
}

TSharedRef<ISequencerTrackEditor> FSequencerImageTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer)
{
	return MakeShareable( new FSequencerImageTrackEditor( OwningSequencer ) );
}

FText FSequencerImageTrackEditor::GetDisplayName() const
{
	return LOCTEXT("SequencerImageTrackEditor_DisplayName", "Image Track");
}

void FSequencerImageTrackEditor::OnInitialize()
{
	// Add Movie Scene Changed Delegate?
}

void FSequencerImageTrackEditor::OnRelease()
{
	// Remove Movie Scene Changed Delegate?
}

void FSequencerImageTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AddTrack", "Image Track"),
		LOCTEXT("AddTooltip", "Adds a new image track that displays slate images on screen."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelViewport.HighResScreenshot"),
		FUIAction(
			FExecuteAction::CreateRaw(this, &FSequencerImageTrackEditor::HandleAddImageTrackMenuEntryExecute)
		)
	);
}

TSharedPtr<SWidget> FSequencerImageTrackEditor::BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& Params,
	const FName& ColumnName)
{
	return FMovieSceneTrackEditor::BuildOutlinerColumnWidget(Params, ColumnName);
}

bool FSequencerImageTrackEditor::HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid)
{
	return FMovieSceneTrackEditor::HandleAssetAdded(Asset, TargetObjectGuid);
}

TSharedRef<ISequencerSection> FSequencerImageTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject,
	UMovieSceneTrack& Track, FGuid ObjectBinding)
{
	check( SupportsType( SectionObject.GetOuter()->GetClass() ) );
	return MakeShareable( new FImageSection(SectionObject, GetSequencer()) );
}

bool FSequencerImageTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
	return Type == USequencerImageTrack::StaticClass();
}

bool FSequencerImageTrackEditor::SupportsSequence(UMovieSceneSequence* InSequence) const
{
	if (InSequence->IsA(ULevelSequence::StaticClass()))
	{
		return true;
	}
	return false;
}

const FSlateBrush* FSequencerImageTrackEditor::GetIconBrush() const
{
	return FAppStyle::GetBrush("LevelViewport.HighResScreenshot");
}

bool FSequencerImageTrackEditor::IsResizable(UMovieSceneTrack* InTrack) const
{
	return true;
}

void FSequencerImageTrackEditor::Resize(float NewSize, UMovieSceneTrack* InTrack)
{
	if (USequencerImageTrack* ImageTrack = Cast<USequencerImageTrack>(InTrack))
	{
		ImageTrack->Modify();
		const int32 MaxNumRows = ImageTrack->GetMaxRowIndex() + 1;
		ImageTrack->SetRowHeight(FMath::RoundToInt(NewSize) / MaxNumRows);
	}
}

bool FSequencerImageTrackEditor::OnAllowDrop(const FDragDropEvent& DragDropEvent,
	FSequencerDragDropParams& DragDropParams)
{
	if (!DragDropParams.Track.IsValid() || !DragDropParams.Track.Get()->IsA(USequencerImageTrack::StaticClass()))
	{
		return false;
	}

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

	if (!Operation.IsValid() || !Operation->IsOfType<FAssetDragDropOp>() )
	{
		return false;
	}
	
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
	if (!SequencerPtr)
	{
		return false;
	}

	UMovieSceneSequence* FocusedSequence = SequencerPtr->GetFocusedMovieSceneSequence();
	if (!FocusedSequence)
	{
		return false;
	}

	TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( Operation );

	for (const FAssetData& AssetData : DragDropOp->GetAssets())
	{
		if (!MovieSceneToolHelpers::IsValidAsset(FocusedSequence, AssetData))
		{
			continue;
		}

		if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
		{
			FFrameNumber LengthInFrames = DefaultLengthInFrames;
			DragDropParams.FrameRange = TRange<FFrameNumber>(DragDropParams.FrameNumber, DragDropParams.FrameNumber + LengthInFrames);
			return true;
		}
	}

	return false;
}

FReply FSequencerImageTrackEditor::OnDrop(const FDragDropEvent& DragDropEvent,
	const FSequencerDragDropParams& DragDropParams)
{
	if (!DragDropParams.Track.IsValid() || !DragDropParams.Track.Get()->IsA(USequencerImageTrack::StaticClass()))
	{
		return FReply::Unhandled();
	}

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();

	if (!Operation.IsValid() || !Operation->IsOfType<FAssetDragDropOp>() )
	{
		return FReply::Unhandled();
	}
	
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
	if (!SequencerPtr)
	{
		return FReply::Unhandled();
	}

	UMovieSceneSequence* FocusedSequence = SequencerPtr->GetFocusedMovieSceneSequence();
	if (!FocusedSequence)
	{
		return FReply::Unhandled();
	}

	USequencerImageTrack* ImageTrack = Cast<USequencerImageTrack>(DragDropParams.Track);

	const FScopedTransaction Transaction(LOCTEXT("DropAssets", "Drop Assets"));

	TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>( Operation );

	FMovieSceneTrackEditor::BeginKeying(DragDropParams.FrameNumber);

	bool bAnyDropped = false;
	for (const FAssetData& AssetData : DragDropOp->GetAssets())
	{
		if (!MovieSceneToolHelpers::IsValidAsset(FocusedSequence, AssetData))
		{
			continue;
		}

		if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
		{
			AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FSequencerImageTrackEditor::AddNewImage, Texture, ImageTrack, DragDropParams.RowIndex));

			bAnyDropped = true;
		}
	}

	FMovieSceneTrackEditor::EndKeying();

	return bAnyDropped ? FReply::Handled() : FReply::Unhandled();
}

FKeyPropertyResult FSequencerImageTrackEditor::AddNewImage(FFrameNumber FrameNumber, UTexture2D* Texture,
	USequencerImageTrack* ImageTrack, int RowIndex)
{
	FKeyPropertyResult KeyPropertyResult;

	UMovieScene* FocusedMovieScene = GetFocusedMovieScene();
	if (FocusedMovieScene->IsReadOnly())
	{
		return KeyPropertyResult;
	}

	FocusedMovieScene->Modify();

	FFindOrCreateRootTrackResult<USequencerImageTrack> TrackResult;
	TrackResult.Track = ImageTrack;
	if (!ImageTrack)
	{
		TrackResult = FindOrCreateRootTrack<USequencerImageTrack>();
		ImageTrack = TrackResult.Track;
	}

	if (ensure(ImageTrack))
	{
		ImageTrack->Modify();

		UMovieSceneSection* NewSection = ImageTrack->AddNewImageOnRow( Texture, FrameNumber, RowIndex );

		if (TrackResult.bWasCreated)
		{
			ImageTrack->SetDisplayName(LOCTEXT("ImageTrackName", "Image Track"));

			if (GetSequencer().IsValid())
			{
				GetSequencer()->OnAddTrack(ImageTrack, FGuid());
			}
		}

		KeyPropertyResult.bTrackModified = true;
		KeyPropertyResult.SectionsCreated.Add(NewSection);
	}

	return KeyPropertyResult;
}

void FSequencerImageTrackEditor::HandleAddImageTrackMenuEntryExecute()
{
	UMovieScene* FocusedMovieScene = GetFocusedMovieScene();

	if (FocusedMovieScene == nullptr)
	{
		return;
	}

	if (FocusedMovieScene->IsReadOnly())
	{
		return;
	}

	const FScopedTransaction Transaction(NSLOCTEXT("Sequencer", "AddImageTrack_Transaction", "Add Image Track"));
	FocusedMovieScene->Modify();
	
	auto NewTrack = FocusedMovieScene->AddTrack<USequencerImageTrack>();
	ensure(NewTrack);

	NewTrack->SetDisplayName(LOCTEXT("ImageTrackName", "Image Track"));

	if (GetSequencer().IsValid())
	{
		GetSequencer()->OnAddTrack(NewTrack, FGuid());
	}
}

FImageSection::FImageSection(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer)
	: Section(InSection)
	, Sequencer(InSequencer)
{
}

// TODO: Look into issues with releasing section
FImageSection::~FImageSection()
{
}

UMovieSceneSection* FImageSection::GetSectionObject()
{
	return &Section;
}

FText FImageSection::GetSectionTitle() const
{
	if (UMovieSceneImageSection* ImageSection = Cast<UMovieSceneImageSection>(&Section))
	{
		if (UTexture2D* Texture = ImageSection->GetTexture())
		{
			// Return the asset name if it exists
			return FText::FromString(Texture->GetName());
		}
		else
		{
			// There is no asset during record so return empty string
			return FText();
		}
	}
	
	return NSLOCTEXT("FAudioSection", "NoImageTitleName", "No Image");
}

FText FImageSection::GetSectionToolTip() const
{
	return ISequencerSection::GetSectionToolTip();
}

float FImageSection::GetSectionHeight(const UE::Sequencer::FViewDensityInfo& ViewDensity) const
{
	if (USequencerImageTrack* Track = Section.GetTypedOuter<USequencerImageTrack>())
	{
		return Track->GetRowHeight();
	}
	return ISequencerSection::GetSectionHeight(ViewDensity);
}

int32 FImageSection::OnPaintSection(FSequencerSectionPainter& InPainter) const
{
	InPainter.LayerId = InPainter.PaintSectionBackground();
	
	if (ThumbnailImageBrush.IsValid())
	{
		FVector2D LocalHeaderSize = InPainter.HeaderGeometry.GetLocalSize();
		if (AspectRatio != 0)
		{
			float SizeY = LocalHeaderSize.Y - 10.f;
			float SizeX = SizeY * AspectRatio;
			float MaxSizeX = LocalHeaderSize.X - 10.f;
			if (SizeX > MaxSizeX)
			{
				SizeX = MaxSizeX;
				SizeY = SizeX * 1/AspectRatio;
			}
			FSlateDrawElement::MakeBox(
				InPainter.DrawElements,
				InPainter.LayerId++,
				InPainter.SectionGeometry.ToPaintGeometry(FVector2D(SizeX, SizeY), FSlateLayoutTransform(FVector2D(5.f, 5.f))),
				ThumbnailImageBrush->GetSlateBrush(),
				InPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect
			);
		}
	}

	return InPainter.LayerId;
}

TSharedRef<SWidget> FImageSection::GenerateSectionWidget()
{
	if (UMovieSceneImageSection* ImageSection = Cast<UMovieSceneImageSection>(&Section))
	{
		if (UTexture2D* Texture = ImageSection->GetTexture())
		{
			ThumbnailImageBrush = FDeferredCleanupSlateBrush::CreateBrush(Texture);
			ThumnailImageWidget = SNew(SImage) // TODO: Check this is cleaned up
				.Image(ThumbnailImageBrush->GetSlateBrush());
			float SizeX = Texture->Source.GetSizeX();
			float SizeY = Texture->Source.GetSizeY();
			AspectRatio = SizeX / SizeY;
			return ThumnailImageWidget.ToSharedRef();
		}
	}

	return SNullWidget::NullWidget;
}

void FImageSection::Tick(const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime,
                         const float InDeltaTime)
{
	// Defer regenerating waveforms if playing or scrubbing
	TSharedPtr<ISequencer> SequencerPin = Sequencer.Pin();
	if (!SequencerPin.IsValid())
	{
		return;
	}
	
	EMovieScenePlayerStatus::Type PlaybackState = SequencerPin->GetPlaybackStatus();

	if (PlaybackState == EMovieScenePlayerStatus::Playing || PlaybackState == EMovieScenePlayerStatus::Scrubbing)
	{
		return;
	}
}

void FImageSection::BeginResizeSection()
{
	ISequencerSection::BeginResizeSection();
}

void FImageSection::ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeTime)
{
	ISequencerSection::ResizeSection(ResizeMode, ResizeTime);
}

void FImageSection::BeginSlipSection()
{
	ISequencerSection::BeginSlipSection();
}

void FImageSection::SlipSection(FFrameNumber SlipTime)
{
	ISequencerSection::SlipSection(SlipTime);
}

TOptional<FFrameTime> FImageSection::GetSectionTime(FSequencerSectionPainter& InPainter) const
{
	return ISequencerSection::GetSectionTime(InPainter);
}

#undef LOCTEXT_NAMESPACE
