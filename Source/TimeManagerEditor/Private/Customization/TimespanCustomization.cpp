#include "Customization/TimespanCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "TimeManagerHelper.h"

#define LOCTEXT_NAMESPACE "TimespanCustomization"

void FTimespanCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;

	HeaderRow
	.NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MaxDesiredWidth(0.0f)
		.MinDesiredWidth(125.0f)
		[
			SAssignNew(TextBox, SEditableTextBox)
				.ClearKeyboardFocusOnCommit(false)
				.IsEnabled(!PropertyHandle->IsEditConst())
				.OnTextChanged(this, &FTimespanCustomization::HandleTimeChanges)
				.OnTextCommitted(this, &FTimespanCustomization::HandleTextBoxTextCommited)
				.SelectAllTextOnCommit(true)
				.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				.Text(this, &FTimespanCustomization::HandleTimeValue)
		];
}

void FTimespanCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}

void FTimespanCustomization::HandleTimeChanges(const FText& NewText)
{
	/**if can be parsed - set is valid*/
	FTimespan Timespan;
	InputValid = UTimeManagerHelper::ParseTimeFromString(NewText.ToString(), Timespan);

	/**correctly moves caret*/
	if(InputValid)
	{
		TextBox->SetForegroundColor(FColor::Black);
	}
	else
	{
		TextBox->SetForegroundColor(FColor::Red);
	}
}
FText FTimespanCustomization::HandleTimeValue() const
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
void FTimespanCustomization::HandleTextBoxTextCommited(const FText& NewText, ETextCommit::Type CommitInfo)
{
	/**if is ok - parse into struct and save as a value*/
	FTimespan ParsedTimespan;
	InputValid = UTimeManagerHelper::ParseTimeFromString(NewText.ToString(), ParsedTimespan);

	if(InputValid && PropertyHandle.IsValid())
	{
		TArray<void*> RawData;

		PropertyHandle->AccessRawData(RawData);
		PropertyHandle->NotifyPreChange();

		for (auto RawDataInstance : RawData)
		{
			*(FTimespan*)RawDataInstance = ParsedTimespan;
		}

		PropertyHandle->NotifyPostChange();
		PropertyHandle->NotifyFinishedChangingProperties();
	}
}

#undef LOCTEXT_NAMESPACE