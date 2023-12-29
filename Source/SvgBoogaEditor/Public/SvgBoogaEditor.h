#pragma once

#include "CoreMinimal.h"
#include "IAssetTypeActions.h"
#include "Modules/ModuleManager.h"

class FSvgBoogaEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
};
