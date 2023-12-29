#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "Factories/Factory.h"
#include "SvgTextureFactory.generated.h"

UCLASS()
class USvgTextureFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	USvgTextureFactory();

	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                   const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
	                                   bool& bOutOperationCanceled) override;
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;

private:
	TSharedPtr<int32> WidthValue;
	TSharedPtr<int32> HeightValue;
	bool ShowImportOptions(USvgImportOptions*& OutImportOptions, const TSharedPtr<FString>& InitialSize = MakeShared<
		                       FString>(TEXT("512x512"))
	);
	TOptional<int32> GetInitialWidth(USvgImportOptions* ImportOptions) const;
	TOptional<int32> GetInitialHeight(USvgImportOptions* ImportOptions) const;
};
