#pragma once

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_SvgTexture : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	// virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	// virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual bool IsImportedAsset() const override;
};
