#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/Texture2D.h"
#include "SvgTexture2D.generated.h"

namespace lunasvg
{
	class Bitmap;
}

UCLASS(BlueprintType)
class SVGBOOGA_API USvgTexture2D : public UObject
{
	GENERATED_BODY()

public:
	USvgTexture2D(const FObjectInitializer& ObjectInitializer);

	bool UpdateTextureFromSvg(const FString& SvgFilePath, const int TextureWidth, const int TextureHeight,
	                          const FLinearColor BackgroundColor);

	UFUNCTION(BlueprintCallable, Category = "SVG")
	UTexture2D* GetTexture() const;

	UFUNCTION(BlueprintCallable, Category = "SVG")
	int32 GetOriginalWidth() const;

	UFUNCTION(BlueprintCallable, Category = "SVG")
	int32 GetOriginalHeight() const;

	UFUNCTION(BlueprintCallable, Category = "SVG")
	FString GetImportPath() const;

	UFUNCTION(BlueprintCallable, Category = "SVG")
	void SetImportPath(FString& NewImportPath);

	UFUNCTION(BlueprintCallable, Category = "SVG")
	float GetAspectRatio();

protected:
	UPROPERTY(VisibleAnywhere, Category = "SVG")
	UTexture2D* Texture;

	UPROPERTY(VisibleAnywhere, Category = "Import Settings")
	FString ImportPath;

	UPROPERTY(VisibleAnywhere, Category = "Import Settings")
	int32 OriginalWidth;

	UPROPERTY(VisibleAnywhere, Category = "Import Settings")
	int32 OriginalHeight;

	UPROPERTY(VisibleAnywhere, Category = "SVG")
	float AspectRatio;

	TSharedPtr<FImage> ConvertBitmapToImage(const lunasvg::Bitmap& Bitmap);
	void UpdateTextureFromImage(const TSharedPtr<FImage>& SourceImage, const int TextureWidth, const int TextureHeight);
	void UpdateTextureFromBitmap(const lunasvg::Bitmap& Bitmap, const int TextureWidth, const int TextureHeight);
	virtual void Serialize(FArchive& Ar) override;
};
