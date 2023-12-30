#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialExpression.h"
#include "SvgTexture2D.h"
#include "MaterialExpressionSvgTextureObject.generated.h"

UCLASS(collapsecategories, hidecategories=Object)
class UMaterialExpressionSvgTextureObject : public UMaterialExpression
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SVG")
	USvgTexture2D* SvgTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MaterialExpressionTextureBase,
		meta = (ShowAsInputPin = "Advanced"))
	TEnumAsByte<EMaterialSamplerType> SamplerType;

	UMaterialExpressionSvgTextureObject(const FObjectInitializer& ObjectInitializer);
	void AutoSetSampleType();
	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const override;

#if WITH_EDITOR
	//~ Begin UMaterialExpression Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual int32 CompilePreview(FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual uint32 GetOutputType(int32 OutputIndex) override;
	//~ End UMaterialExpression Interface
#endif
};
