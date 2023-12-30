#include "SvgTextureFactory.h"

#include "SvgImportOptions.h"
#include "SvgTexture2D.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"

USvgTextureFactory::USvgTextureFactory()
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;
	SupportedClass = USvgTexture2D::StaticClass();

	Formats.Add(TEXT("svg;Scalable Vector Graphics"));
}

bool USvgTextureFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("svg"))
	{
		return true;
	}
	return false;
}

UObject* USvgTextureFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                               const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
                                               bool& bOutOperationCanceled)
{
	USvgImportOptions* ImportOptions = nullptr;
	if (!ShowImportOptions(ImportOptions))
	{
		bOutOperationCanceled = true;
		return nullptr;
	}

	USvgTexture2D* NewTexture = NewObject<USvgTexture2D>(InParent, InClass, InName, Flags);

	if (NewTexture->UpdateTextureFromSvg(Filename, ImportOptions->Width, ImportOptions->Height,
	                                     ImportOptions->BackgroundColor))
	{
		FAssetRegistryModule::AssetCreated(NewTexture);
		NewTexture->MarkPackageDirty();

		if (GIsEditor)
		{
			UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
			AssetEditorSubsystem->OpenEditorForAsset(NewTexture);
		}

		return NewTexture;
	}

	return nullptr;
}


bool USvgTextureFactory::ShowImportOptions(USvgImportOptions*& OutImportOptions, const TSharedPtr<FString>& InitialSize)
{
	OutImportOptions = NewObject<USvgImportOptions>(GetTransientPackage(), NAME_None, RF_NoFlags);

	const TArray<TSharedPtr<FString>> SizeOptions = {
		MakeShared<FString>(TEXT("32x32")),
		MakeShared<FString>(TEXT("64x64")),
		MakeShared<FString>(TEXT("128x128")),
		MakeShared<FString>(TEXT("256x256")),
		MakeShared<FString>(TEXT("512x512")),
		MakeShared<FString>(TEXT("1024x1024")),
		MakeShared<FString>(TEXT("2048x2048")),
		MakeShared<FString>(TEXT("4096x4096")),
		MakeShared<FString>(TEXT("8192x8192"))
	};

	WidthValue = MakeShared<int32>(OutImportOptions->Width);
	HeightValue = MakeShared<int32>(OutImportOptions->Height);

	bool bIsValidInitialSize = false;
	for (const TSharedPtr<FString>& SizeOption : SizeOptions)
	{
		if (SizeOption.IsValid() && InitialSize.IsValid() && *SizeOption == *InitialSize)
		{
			bIsValidInitialSize = true;
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("Invalid InitialSize provided to USvgTextureFactory::ShowImportOptions"));
	}


	TSharedPtr<FString> CurrentValue = bIsValidInitialSize ? InitialSize : SizeOptions[4];
	TSharedPtr<SComboBox<TSharedPtr<FString>>> SizeComboBox;
	bool bUserClickedOk = false;

	const TSharedRef<SWindow> ImportOptionsWindow = SNew(SWindow)
        .Title(FText::FromString("SVG Import Options"))
        .SizingRule(ESizingRule::Autosized);

	ImportOptionsWindow->SetContent
	(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(8, 2, 8, 2)
		[
			SNew(STextBlock).Text(FText::FromString("Size:"))
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(8, 2, 8, 4)
		[
			SAssignNew(SizeComboBox, SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&SizeOptions)
	.OnSelectionChanged_Lambda([&CurrentValue, &OutImportOptions](TSharedPtr<FString> NewValue, ESelectInfo::Type)
			                                                        {
				                                                        if (NewValue.IsValid())
				                                                        {
					                                                        *CurrentValue = *NewValue;
					                                                        const FString SelectedSize = *CurrentValue;
					                                                        const int32 xIndex = SelectedSize.Find(
						                                                        TEXT("x"));
					                                                        if (xIndex != INDEX_NONE)
					                                                        {
						                                                        const FString WidthStr = SelectedSize.
							                                                        Left(xIndex);
						                                                        const FString HeightStr = SelectedSize.
							                                                        Mid(xIndex + 1);
						                                                        OutImportOptions->Width =
							                                                        FCString::Atoi(*WidthStr);
						                                                        OutImportOptions->Height =
							                                                        FCString::Atoi(*HeightStr);
					                                                        }
				                                                        }
			                                                        })
	.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
			                                                        {
				                                                        return SNew(STextBlock).Text(
					                                                        FText::FromString(*Item));
			                                                        })
	.InitiallySelectedItem(CurrentValue)
	.Content()
			[
				SNew(STextBlock).Text_Lambda([CurrentValue]
				{
					return FText::FromString(*CurrentValue);
				})
			]
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(8, 2, 8, 2)
		[
			SNew(STextBlock).Text(FText::FromString("Background Color:"))
		]
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(8, 2, 8, 2)
		[
			SNew(SColorBlock)
			.Color(OutImportOptions->BackgroundColor)
			.Size(FVector2D(100, 30))
			.ShowBackgroundForAlpha(true)
			.OnMouseButtonDown_Lambda([&OutImportOptions](const FGeometry&, const FPointerEvent&)
			                 {
				                 FColorPickerArgs PickerArgs;
				                 PickerArgs.bIsModal = true;
				                 PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateLambda(
					                 [&OutImportOptions](const FLinearColor& NewColor)
					                 {
						                 OutImportOptions->BackgroundColor = NewColor;
						                 // Update your variable when color changes
					                 });
				                 PickerArgs.InitialColor = OutImportOptions->BackgroundColor;
				                 PickerArgs.ParentWidget = SNew(SColorPicker);
				                 OpenColorPicker(PickerArgs);
				                 return FReply::Handled();
			                 })
		] + SVerticalBox::Slot()
		    .AutoHeight()
		    .HAlign(HAlign_Right)
		    .Padding(8, 8, 8, 2)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .Padding(2)
			[
				SNew(SButton)
                .Text(FText::FromString("OK"))
                .OnClicked_Lambda([&]
				             {
					             bUserClickedOk = true;
					             ImportOptionsWindow->RequestDestroyWindow();
					             return FReply::Handled();
				             })
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .Padding(2)
			[
				SNew(SButton)
                .Text(FText::FromString("Cancel"))
                .OnClicked_Lambda([&]
				             {
					             ImportOptionsWindow->RequestDestroyWindow();
					             return FReply::Handled();
				             })
			]
		]
	);

	GEditor->EditorAddModalWindow(ImportOptionsWindow);

	return bUserClickedOk;
}

TOptional<int32> USvgTextureFactory::GetInitialWidth(USvgImportOptions* ImportOptions) const
{
	return ImportOptions->Width;
}

TOptional<int32> USvgTextureFactory::GetInitialHeight(USvgImportOptions* ImportOptions) const
{
	return ImportOptions->Height;
}

bool USvgTextureFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	const USvgTexture2D* SvgTexture = Cast<USvgTexture2D>(Obj);
	if (SvgTexture && SvgTexture->GetImportPath().Len() > 0)
	{
		OutFilenames.Add(SvgTexture->GetImportPath());
		return true;
	}
	return false;
}

EReimportResult::Type USvgTextureFactory::Reimport(UObject* Obj)
{
	USvgTexture2D* SvgTexture = Cast<USvgTexture2D>(Obj);
	if (!SvgTexture)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = SvgTexture->GetImportPath();
	if (!FPaths::FileExists(Filename))
	{
		return EReimportResult::Failed;
	}

	const auto SizeString =
		MakeShared<FString>(FString::Format(TEXT("{0}x{1}"), {
			                                    SvgTexture->GetTexture()->GetSurfaceWidth(),
			                                    SvgTexture->GetTexture()->GetSurfaceHeight()
		                                    }));

	USvgImportOptions* ImportOptions = nullptr;
	if (!ShowImportOptions(ImportOptions, SizeString))
	{
		return EReimportResult::Cancelled;
	}

	if (SvgTexture->UpdateTextureFromSvg(Filename, ImportOptions->Width, ImportOptions->Height,
	                                     ImportOptions->BackgroundColor))
	{
		return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}

void USvgTextureFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	if (USvgTexture2D* SvgTexture = Cast<USvgTexture2D>(Obj))
	{
		if (NewReimportPaths.Num() > 0)
		{
			FString NewFolderPath = NewReimportPaths[0];
			FString OriginalFilename = FPaths::GetCleanFilename(SvgTexture->GetImportPath());
			FString NewImportPath = FPaths::Combine(NewFolderPath, OriginalFilename);
			SvgTexture->SetImportPath(NewImportPath);
		}
	}
}
