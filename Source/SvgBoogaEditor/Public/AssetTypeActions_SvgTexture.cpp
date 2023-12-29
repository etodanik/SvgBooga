#include "AssetTypeActions_SvgTexture.h"

#include "SvgTexture2D.h"

FText FAssetTypeActions_SvgTexture::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_SvgTexture", "SVG Texture");
}


FColor FAssetTypeActions_SvgTexture::GetTypeColor() const
{
	return FColor(128, 0, 128);
}

UClass* FAssetTypeActions_SvgTexture::GetSupportedClass() const
{
	return USvgTexture2D::StaticClass();
}

uint32 FAssetTypeActions_SvgTexture::GetCategories()
{
	return EAssetTypeCategories::Textures;
}

// bool FAssetTypeActions_SvgTexture::HasActions(const TArray<UObject*>& InObjects) const
// {
// 	return true;
// }
//
//
// void FAssetTypeActions_SvgTexture::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
// {
// 	const FText ButtonLabel = FText::FromString("Reimport");
// 	const FText ButtonToolTip = FText::FromString("Re-imports the asset from disk");
// 	auto BcgAssets = GetTypedWeakObjectPtrs<USvgTexture2D>(InObjects);
// 	const auto TDelegateExecuteAction = FExecuteAction::CreateSP(this, &FAssetTypeActions_SvgTexture::ReImport,
// 	                                                             BcgAssets);
// 	const auto UIAction = FUIAction(TDelegateExecuteAction);
// 	MenuBuilder.AddMenuEntry(ButtonLabel, ButtonToolTip, FSlateIcon(), UIAction);
// }

bool FAssetTypeActions_SvgTexture::IsImportedAsset() const
{
	return true;
}
