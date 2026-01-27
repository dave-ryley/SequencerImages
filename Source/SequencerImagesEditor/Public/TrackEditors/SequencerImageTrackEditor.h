#pragma once
#include "MovieSceneTrackEditor.h"
#include "SequencerImageTrack.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "TrackEditorThumbnail/TrackEditorThumbnail.h"

/**
 * Tools for image tracks
 */
class SEQUENCERIMAGESEDITOR_API FSequencerImageTrackEditor
	: public FMovieSceneTrackEditor
{
public:

	/**
	 * Constructor
	 *
	 * @param InSequencer The sequencer instance to be used by this tool
	 */
	FSequencerImageTrackEditor(TSharedRef<ISequencer> InSequencer);
	
	/** Virtual destructor. */
	virtual ~FSequencerImageTrackEditor();
	
	/**
	 * Creates an instance of this class.  Called by a sequencer 
	 *
	 * @param OwningSequencer The sequencer instance to be used by this tool
	 * @return The new instance of this class
	 */
	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer);

public:

	// ISequencerTrackEditor interface

	virtual FText GetDisplayName() const override;
	virtual void OnInitialize() override;
	virtual void OnRelease() override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	virtual TSharedPtr<SWidget> BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& Params, const FName& ColumnName) override;
	virtual bool HandleAssetAdded(UObject* Asset, const FGuid& TargetObjectGuid) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;
	virtual bool SupportsSequence(UMovieSceneSequence* InSequence) const override;
	virtual const FSlateBrush* GetIconBrush() const override;
	virtual bool IsResizable(UMovieSceneTrack* InTrack) const override;
	virtual void Resize(float NewSize, UMovieSceneTrack* InTrack) override;
	virtual bool OnAllowDrop(const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams) override;
	virtual FReply OnDrop(const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams) override;

private:
	FKeyPropertyResult AddNewImage(FFrameNumber FrameNumber, UTexture2D* Texture, USequencerImageTrack* ImageTrack, int RowIndex);
	void HandleAddImageTrackMenuEntryExecute();
	FFrameNumber DefaultLengthInFrames = 60;
};

/**
 * Class for image sections, handles drawing of thumbnail previews.
 */
class SEQUENCERIMAGESEDITOR_API FImageSection
	: public ISequencerSection
	, public TSharedFromThis<FImageSection>
{
public:
	/** Constructor. */
	FImageSection(UMovieSceneSection& InSection, TWeakPtr<ISequencer> InSequencer);

	/** Virtual destructor. */
	virtual ~FImageSection();

public:

	// ISequencerSection interface

	virtual UMovieSceneSection* GetSectionObject() override;
	virtual FText GetSectionTitle() const override;
	virtual FText GetSectionToolTip() const override;
	virtual float GetSectionHeight(const UE::Sequencer::FViewDensityInfo& ViewDensity) const override;
	virtual int32 OnPaintSection(FSequencerSectionPainter& InPainter) const override;
	virtual TSharedRef<SWidget> GenerateSectionWidget() override;
	virtual void Tick(const FGeometry& AllottedGeometry, const FGeometry& ParentGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual void BeginResizeSection() override;
	virtual void ResizeSection(ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeTime) override;
	virtual void BeginSlipSection() override;
	virtual void SlipSection(FFrameNumber SlipTime) override;
	virtual TOptional<FFrameTime> GetSectionTime(FSequencerSectionPainter& InPainter) const override;

private:
	/** The section we are visualizing. */
	UMovieSceneSection& Section;

	TWeakPtr<ISequencer> Sequencer;
	TSharedPtr<FDeferredCleanupSlateBrush> ThumbnailImageBrush;
	float AspectRatio = 1.f;
	TSharedPtr<SImage> ThumnailImageWidget;
};
