#pragma once

#include "ThumbnailRendering/DefaultSizedThumbnailRenderer.h"
#include "SvgTexture2DThumbnailRenderer.generated.h"

UCLASS()
class USvgTexture2DThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
	GENERATED_BODY()

public:
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport,
	                  FCanvas* Canvas, bool bAdditionalViewFamily) override;
};
