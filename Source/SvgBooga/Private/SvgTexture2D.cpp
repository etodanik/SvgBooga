#include "SvgTexture2D.h"
#include "LunaSvg/lunasvg.h"
#include "Engine/Texture2D.h"
#include "RenderUtils.h"

USvgTexture2D::USvgTexture2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Texture = ObjectInitializer.CreateDefaultSubobject<UTexture2D>(this, TEXT("Texture"));
}

bool USvgTexture2D::UpdateTextureFromSvg(const FString& SvgFilePath, int width, int height)
{
	const std::unique_ptr<lunasvg::Document> document = lunasvg::Document::loadFromFile(TCHAR_TO_UTF8(*SvgFilePath));
	if (!document)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load SVG file: %s"), *SvgFilePath);
		return false;
	}

	ImportPath = SvgFilePath;
	OriginalWidth = document->width();
	OriginalHeight = document->height();
	AspectRatio = static_cast<float>(OriginalWidth) / OriginalHeight;

	const lunasvg::Bitmap bitmap = document->renderToBitmap(width, height);
	if (!bitmap.valid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to render SVG to bitmap."));
		return false;
	}

	UpdateTextureFromBitmap(bitmap);
	return true;
}

void USvgTexture2D::UpdateTextureFromBitmap(const lunasvg::Bitmap& Bitmap)
{
	if (!Bitmap.valid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Bitmap data."));
		return;
	}

	if (!IsInGameThread())
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateTextureFromBitmap must be called on the game thread."));
		return;
	}

	if (!Texture)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture is not initialized."));
		return;
	}

	FTexturePlatformData* PlatformData = Texture->GetPlatformData();
	if (!PlatformData || PlatformData->SizeX != Bitmap.width() || PlatformData->SizeY != Bitmap.height())
	{
		PlatformData = new FTexturePlatformData();
		PlatformData->SizeX = Bitmap.width();
		PlatformData->SizeY = Bitmap.height();
		PlatformData->PixelFormat = PF_B8G8R8A8;

		FTexture2DMipMap* NewMip = new FTexture2DMipMap();
		NewMip->SizeX = Bitmap.width();
		NewMip->SizeY = Bitmap.height();
		PlatformData->Mips.Add(NewMip);

		Texture->SetPlatformData(PlatformData);
	}

	if (PlatformData->Mips.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Texture has no mipmaps."));
		return;
	}

	FTexture2DMipMap& Mip = PlatformData->Mips[0];

	if (Mip.BulkData.IsLocked())
	{
		UE_LOG(LogTemp, Warning, TEXT("BulkData is already locked."));
		return;
	}

	Mip.BulkData.Lock(LOCK_READ_WRITE);
	Mip.BulkData.Realloc(Bitmap.width() * Bitmap.height() * 4);
	Mip.BulkData.Unlock();

	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to lock MipMap data for writing."));
		return;
	}

	const int32 Stride = Bitmap.width() * 4;
	for (uint32 y = 0; y < Bitmap.height(); ++y)
	{
		uint32* DestPtr = reinterpret_cast<uint32*>(static_cast<uint8*>(Data) + y * Stride);
		const uint32* SrcPtr = reinterpret_cast<const uint32*>(Bitmap.data() + y * Bitmap.stride());
		for (uint32 x = 0; x < Bitmap.width(); ++x)
		{
			const uint32 Pixel = SrcPtr[x];
			DestPtr[x] = Pixel & 0xFF00FF00 | (Pixel & 0xFF0000) >> 16 | (Pixel & 0xFF) << 16;
		}
	}

	Mip.BulkData.Unlock();
	Texture->Source.Init(Bitmap.width(), Bitmap.height(), 1, 1, TSF_BGRA8, Bitmap.data());
	Texture->AssetImportData = nullptr;
	Texture->UpdateResource();
}

void USvgTexture2D::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaving() || Ar.IsLoading())
	{
		if (Texture)
		{
			Texture->Serialize(Ar);
		}

		Ar << OriginalWidth;
		Ar << OriginalHeight;
		Ar << ImportPath;
		Ar << AspectRatio;
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
