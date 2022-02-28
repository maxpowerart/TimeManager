#include "Customization/DateTimeCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "TimeManagerHelper.h"

#define LOCTEXT_NAMESPACE "DateTimeCustomization"

void FDateTimeCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;

	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.Padding(0, 5,0,0)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
					.Text(LOCTEXT("ValueDate", "Date: "))
				]
					
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(DateBox, SEditableTextBox)
						.ClearKeyboardFocusOnCommit(false)
						.IsEnabled(!PropertyHandle->IsEditConst())
						.OnTextChanged(this, &FDateTimeCustomization::HandleDateChanges)
						.OnTextCommitted(this, &FDateTimeCustomization::HandleDateTextCommited)
						.SelectAllTextOnCommit(true)
						.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
						.Text(this, &FDateTimeCustomization::HandleDateValue)
				]
		]

		+SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.Padding(0, 5,0,0)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
					.Text(LOCTEXT("ValueTime", "Time: "))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()[
					SAssignNew(TimeBox, SEditableTextBox)
						.ClearKeyboardFocusOnCommit(false)
						.IsEnabled(!PropertyHandle->IsEditConst())
						.OnTextChanged(this, &FDateTimeCustomization::HandleTimeChanges)
						.OnTextCommitted(this, &FDateTimeCustomization::HandleTimeTextCommited)
						.SelectAllTextOnCommit(true)
						.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
						.Text(this, &FDateTimeCustomization::HandleTimeValue)
				]
		]
	];
}

void FDateTimeCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FDateTimeCustomization::HandleTimeChanges(const FText& NewText)
{
	/**if can be parsed - set is valid*/
	FTimespan Timespan;
	bInputTimeValid = UTimeManagerHelper::ParseTimeFromString(NewText.ToString(), Timespan);

	/**correctly moves caret*/
	if(bInputTimeValid)
	{
		TimeBox->SetForegroundColor(FColor::Black);
	}
	else
	{
		TimeBox->SetForegroundColor(FColor::Red);
	}
}
FText FDateTimeCustomization::HandleTimeValue() const
{
	TArray<void*> RawData;
	PropertyHandle->AccessRawData(RawData);
	
	if (RawData.Num() != 1)
	{
		return LOCTEXT("MultipleValues", "Multiple Values");
	}
	
	if(RawData[0] == nullptr)
	{
		return FText::GetEmpty();
	}

	FString ValueString;

	if (!((FTimespan*)RawData[0])->ExportTextItem(ValueString, FTimespan::Zero(), NULL, 0, NULL))
	{
		return FText::GetEmpty();
	}
	ValueString = UTimeManagerHelper::TimeToString(*(FTimespan*)RawData[0]);
	return FText::FromString(ValueString);
}
void FDateTimeCustomization::HandleTimeTextCommited(const FText& NewText, ETextCommit::Type CommitInfo)
{
	/**if is ok - parse into struct and save as a value*/
	FTimespan ParsedTimespan;
	bInputTimeValid = UTimeManagerHelper::ParseTimeFromString(NewText.ToString(), ParsedTimespan);

	FDateTime ParsedDate;
	bInputDateValid = UTimeManagerHelper::ParseDateFromString(DateBox->GetText().ToString(), ParsedDate);

	ParsedDate += ParsedTimespan;
	if(bInputDateValid && bInputTimeValid && PropertyHandle.IsValid())
	{
		TArray<void*> RawData;

		PropertyHandle->AccessRawData(RawData);
		PropertyHandle->NotifyPreChange();

		for (auto RawDataInstance : RawData)
		{
			*(FDateTime*)RawDataInstance = ParsedDate;
		}

		PropertyHandle->NotifyPostChange();
		PropertyHandle->NotifyFinishedChangingProperties();
	}
}

void FDateTimeCustomization::HandleDateChanges(const FText& NewText)
{
	/**if can be parsed - set is valid*/
	FDateTime Date;
	bInputDateValid = UTimeManagerHelper::ParseDateFromString(NewText.ToString(), Date);

	/**correctly moves caret*/
	if(bInputDateValid)
	{
		DateBox->SetForegroundColor(FColor::Black);
	}
	else
	{
		DateBox->SetForegroundColor(FColor::Red);
	}
}
FText FDateTimeCustomization::HandleDateValue() const
{
	TArray<void*> RawData;
	PropertyHandle->AccessRawData(RawData);
	
	if (RawData.Num() != 1)
	{
		return LOCTEXT("MultipleValues", "Multiple Values");
	}
	
	if(RawData[0] == nullptr)
	{
		return FText::GetEmpty();
	}

	FString ValueString;

	if (!((FDateTime*)RawData[0])->ExportTextItem(ValueString, FDateTime(), NULL, 0, NULL))
	{
		return FText::GetEmpty();
	}
	ValueString = UTimeManagerHelper::DateToString(*(FDateTime*)RawData[0]);
	return FText::FromString(ValueString);
}

void FDateTimeCustomization::HandleDateTextCommited(const FText& NewText, ETextCommit::Type CommitInfo)
{
	/**if is ok - parse into struct and save as a value*/
	FTimespan ParsedTimespan;
	bInputTimeValid = UTimeManagerHelper::ParseTimeFromString(TimeBox->GetText().ToString(), ParsedTimespan);

	FDateTime ParsedDate;
	bInputDateValid = UTimeManagerHelper::ParseDateFromString(NewText.ToString(), ParsedDate);

	ParsedDate += ParsedTimespan;

	if(bInputTimeValid && bInputDateValid && PropertyHandle.IsValid())
	{
		TArray<void*> RawData;

		PropertyHandle->AccessRawData(RawData);
		PropertyHandle->NotifyPreChange();

		for (auto RawDataInstance : RawData)
		{
			*(FDateTime*)RawDataInstance = ParsedDate;
		}

		PropertyHandle->NotifyPostChange();
		PropertyHandle->NotifyFinishedChangingProperties();
	}
}

#undef LOCTEXT_NAMESPACE
