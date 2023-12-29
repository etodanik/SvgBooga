#include "SvgBoogaEditor.h"

#include "Modules/ModuleManager.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_SvgTexture.h"
#include "SvgTexture2D.h"
#include "SvgTexture2DThumbnailRenderer.h"

#define LOCTEXT_NAMESPACE "FSvgBoogaEditorModule"

void FSvgBoogaEditorModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	const TSharedRef<IAssetTypeActions> SvgTextureAssetTypeActions = MakeShareable(new FAssetTypeActions_SvgTexture());
	AssetTools.RegisterAssetTypeActions(SvgTextureAssetTypeActions);
	CreatedAssetTypeActions.Add(SvgTextureAssetTypeActions);
	UThumbnailManager::Get().RegisterCustomRenderer(USvgTexture2D::StaticClass(),
	                                                USvgTexture2DThumbnailRenderer::StaticClass());
}

void FSvgBoogaEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto& TypeAction : CreatedAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(TypeAction.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSvgBoogaEditorModule, SvgBoogaEditor)
