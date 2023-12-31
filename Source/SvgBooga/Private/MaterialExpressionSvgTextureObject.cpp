#include "../Public/MaterialExpressionSvgTextureObject.h"

#include "EditorSupportDelegates.h"
#include "MaterialCompiler.h"

UMaterialExpressionSvgTextureObject::UMaterialExpressionSvgTextureObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , SvgTexture(nullptr)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_Texture;
		FText NAME_Functions;
		FText NAME_SVG;

		FConstructorStatics()
			: NAME_Texture(NSLOCTEXT("MaterialExpressionSvgTextureObject", "Texture", "Texture"))
			  , NAME_Functions(NSLOCTEXT("MaterialExpressionSvgTextureObject", "Functions", "Functions"))
			  , NAME_SVG(NSLOCTEXT("MaterialExpressionSvgTextureObject", "SVG", "SVG"))
		{
		}
	};

	static FConstructorStatics ConstructorStatics;


#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_Texture);
	MenuCategories.Add(ConstructorStatics.NAME_Functions);
	MenuCategories.Add(ConstructorStatics.NAME_SVG);

	Outputs.Empty();
	Outputs.Add(FExpressionOutput(TEXT("")));

	bCollapsed = false;
#endif // WITH_EDITORONLY_DATA
}

void UMaterialExpressionSvgTextureObject::AutoSetSampleType()
{
	SamplerType = SAMPLERTYPE_Color;
}

UObject* UMaterialExpressionSvgTextureObject::GetReferencedTexture() const
{
	return SvgTexture ? SvgTexture->GetTexture() : nullptr;
}

bool UMaterialExpressionSvgTextureObject::CanReferenceTexture() const
{
	return true;
}

#if WITH_EDITOR
void UMaterialExpressionSvgTextureObject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("Texture"))
	{
		if (SvgTexture && SvgTexture->GetTexture())
		{
			AutoSetSampleType();
			FEditorSupportDelegates::ForcePropertyWindowRebuild.Broadcast(this);
		}
	}
}

int32 UMaterialExpressionSvgTextureObject::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (!SvgTexture || !SvgTexture->IsValidLowLevel())
	{
		return CompilerError(Compiler, TEXT("Requires valid SVG texture"));
	}

	UTexture2D* Texture = SvgTexture->GetTexture();
	if (!Texture || !Texture->IsValidLowLevel())
	{
		return CompilerError(Compiler, TEXT("Requires valid texture initialized on SVG texture asset"));
	}

	return Compiler->Texture(Texture, SamplerType);
}

int32 UMaterialExpressionSvgTextureObject::CompilePreview(FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (!SvgTexture || !SvgTexture->IsValidLowLevel())
	{
		return CompilerError(Compiler, TEXT("Requires valid SVG texture"));
	}

	UTexture2D* Texture = SvgTexture->GetTexture();
	if (!Texture || !Texture->IsValidLowLevel())
	{
		return CompilerError(Compiler, TEXT("Requires valid texture initialized on SVG texture asset"));
	}

	return Compiler->TextureSample(Compiler->Texture(Texture, SamplerType),
	                               Compiler->TextureCoordinate(0, false, false),
	                               SAMPLERTYPE_Color);
}

void UMaterialExpressionSvgTextureObject::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("SVG Texture Object"));
}

uint32 UMaterialExpressionSvgTextureObject::GetOutputType(int32 OutputIndex)
{
	return MCT_Texture2D;
}

#endif //WITH_EDITOR
