#include "SvgTexture2D.h"

#include "ImageUtils.h"
#if WITH_EDITOR
#include "LunaSvg/lunasvg.h"
#endif
#include "Engine/Texture2D.h"
#include "RenderUtils.h"

uint32 ConvertFLinearColorToInteger(const FLinearColor& Color)
{
	const uint8 R = static_cast<uint8>(FMath::Clamp(Color.R * 255.0f, 0.0f, 255.0f));
	const uint8 G = static_cast<uint8>(FMath::Clamp(Color.G * 255.0f, 0.0f, 255.0f));
	const uint8 B = static_cast<uint8>(FMath::Clamp(Color.B * 255.0f, 0.0f, 255.0f));
	const uint8 A = static_cast<uint8>(FMath::Clamp(Color.A * 255.0f, 0.0f, 255.0f));

	return static_cast<uint32>(R) << 24 | static_cast<uint32>(G) << 16 | static_cast<uint32>(B) << 8 | static_cast<
		uint32>(A);
}

USvgTexture2D::USvgTexture2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Texture = ObjectInitializer.CreateDefaultSubobject<UTexture2D>(this, TEXT("Texture"));
	ScaledImage = MakeShareable<FImage>(new FImage());
}

#if WITH_EDITOR
bool USvgTexture2D::UpdateTextureFromSvg(const FString& SvgFilePath, const int TextureWidth, const int TextureHeight,
                                         const FLinearColor InBackgroundColor = FLinearColor::Transparent)
{
	BackgroundColor = InBackgroundColor;
	const std::unique_ptr<lunasvg::Document> Document = lunasvg::Document::loadFromFile(TCHAR_TO_UTF8(*SvgFilePath));
	if (!Document)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load SVG file: %s"), *SvgFilePath);
		return false;
	}

	ImportPath = SvgFilePath;
	OriginalWidth = Document->width();
	OriginalHeight = Document->height();
	AspectRatio = static_cast<float>(OriginalWidth) / OriginalHeight;
	const float TargetAspectRatio = static_cast<float>(TextureWidth) / TextureHeight;

	int RenderWidth, RenderHeight;
	if (AspectRatio >= TargetAspectRatio)
	{
		RenderHeight = TextureHeight;
		RenderWidth = static_cast<int>(TextureHeight * AspectRatio);
	}
	else
	{
		RenderWidth = TextureWidth;
		RenderHeight = static_cast<int>(TextureWidth / AspectRatio);
	}

	const lunasvg::Bitmap Bitmap = Document->renderToBitmap(RenderWidth, RenderHeight,
	                                                        ConvertFLinearColorToInteger(BackgroundColor));
	if (!Bitmap.valid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to render SVG to bitmap."));
		return false;
	}

	UpdateTextureFromBitmap(Bitmap, TextureWidth, TextureHeight);
	return true;
}

TSharedPtr<FImage> USvgTexture2D::ConvertBitmapToImage(const lunasvg::Bitmap& Bitmap)
{
	TSharedPtr<FImage> Image = MakeShared<FImage>();
	if (!Bitmap.valid())
	{
		return Image;
	}

	const uint32 BitmapWidth = Bitmap.width();
	const uint32 BitmapHeight = Bitmap.height();
	const uint32 Stride = Bitmap.stride();

	Image->Init(BitmapWidth, BitmapHeight, ERawImageFormat::BGRA8, EGammaSpace::sRGB);
	const uint8* BitmapData = Bitmap.data();

	// We're converting lunasvg ARGB32 Premultiplied to BGRA8
	for (uint32 Y = 0; Y < BitmapHeight; ++Y)
	{
		for (uint32 X = 0; X < BitmapWidth; ++X)
		{
			const uint32 PixelIndex = Y * Stride + X * 4;
			const uint8 A = BitmapData[PixelIndex + 3];
			const uint8 R = BitmapData[PixelIndex + 2];
			const uint8 G = BitmapData[PixelIndex + 1];
			const uint8 B = BitmapData[PixelIndex + 0];

			const uint32 DestIndex = (Y * BitmapWidth + X) * 4;
			Image->RawData[DestIndex + 0] = B;
			Image->RawData[DestIndex + 1] = G;
			Image->RawData[DestIndex + 2] = R;
			Image->RawData[DestIndex + 3] = A;
		}
	}

	return Image;
}

void USvgTexture2D::UpdateTextureFromImage(const TSharedPtr<FImage>& SourceImage, const int TextureWidth,
                                           const int TextureHeight)
{
	if (!IsInGameThread())
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateTextureFromImage must be called on the game thread."));
		return;
	}

	if (!SourceImage)
	{
		UE_LOG(LogTemp, Error, TEXT("SourceImage is not valid."));
		return;
	}

	ScaledImage = MakeShared<FImage>();
	ScaledImage->Init(TextureWidth, TextureHeight, SourceImage->Format, SourceImage->GammaSpace);
	FImageUtils::ImageResize(SourceImage->SizeX, SourceImage->SizeY, SourceImage->AsBGRA8(), TextureWidth,
	                         TextureHeight,
	                         ScaledImage->AsBGRA8(), false, false);
	if (Texture)
	{
		Texture->Rename(TEXT("TextureGarbage"));
		Texture->MarkAsGarbage();
		Texture->RemoveFromRoot();
		Texture->ReleaseResource();
		Texture = nullptr;
		GEngine->ForceGarbageCollection(true);
	}

	Texture = FImageUtils::CreateTexture2DFromImage(*ScaledImage);
	Texture->Rename(TEXT("Texture"), this);
	Texture->UpdateResource();
}


void USvgTexture2D::UpdateTextureFromBitmap(const lunasvg::Bitmap& Bitmap, const int TextureWidth,
                                            const int TextureHeight)
{
	return UpdateTextureFromImage(ConvertBitmapToImage(Bitmap), TextureWidth, TextureHeight);
}
#endif

void USvgTexture2D::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << OriginalWidth;
	Ar << OriginalHeight;
	Ar << ImportPath;
	Ar << AspectRatio;
	Ar << Width;
	Ar << Height;
	Ar << BackgroundColor;
	Ar << ScaledImage->SizeX;
	Ar << ScaledImage->SizeY;
	Ar << ScaledImage->NumSlices;

	if (Ar.IsSaving())
	{
		int32 FormatInt = ScaledImage->Format;
		Ar << FormatInt;
	}
	if (Ar.IsLoading())
	{
		int32 FormatInt;
		Ar << FormatInt;
		ScaledImage->Format = static_cast<ERawImageFormat::Type>(FormatInt);
	}

	Ar << ScaledImage->GammaSpace;
	Ar << ScaledImage->RawData;

	if (Ar.IsLoading())
	{
		Texture = FImageUtils::CreateTexture2DFromImage(*ScaledImage);
		Texture->Rename(TEXT("Texture"), this);
		Texture->UpdateResource();
	}
}

UTexture2D* USvgTexture2D::GetTexture() const { return Texture; }

int32 USvgTexture2D::GetOriginalWidth() const { return OriginalWidth; }

int32 USvgTexture2D::GetOriginalHeight() const { return OriginalHeight; }

FString USvgTexture2D::GetImportPath() const { return ImportPath; }

void USvgTexture2D::SetImportPath(FString& NewImportPath)
{
	ImportPath = NewImportPath;
}

float USvgTexture2D::GetAspectRatio()
{
	return AspectRatio;
}
