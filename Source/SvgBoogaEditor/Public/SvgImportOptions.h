#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SvgImportOptions.generated.h"

UCLASS()
class USvgImportOptions : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "SVG Import")
	int32 Width = 512;

	UPROPERTY(EditAnywhere, Category = "SVG Import")
	int32 Height = 512;
};
