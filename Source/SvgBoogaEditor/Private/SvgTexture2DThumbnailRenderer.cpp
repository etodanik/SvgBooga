#include "SvgTexture2DThumbnailRenderer.h"

#include "CanvasItem.h"
#include "SvgTexture2D.h"
#include "CanvasRender.h"

void USvgTexture2DThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height,
                                          FRenderTarget* Viewport,
                                          FCanvas* Canvas, bool bAdditionalViewFamily)
{
	if (!Object)
	{
		return;
	}

	if (const auto Texture = Cast<USvgTexture2D>(Object)->GetTexture())
	{
		if (const FTexture* TextureResource = Texture->GetResource())
		{
			FCanvasTileItem TileItem(FVector2D(X, Y), TextureResource, FVector2D(Width, Height), FLinearColor::White);
			TileItem.BlendMode = SE_BLEND_Opaque;
			Canvas->DrawItem(TileItem);
		}
	}
}
