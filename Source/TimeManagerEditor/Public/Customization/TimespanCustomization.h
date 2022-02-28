#pragma once
 
#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FTimespanCustomization : public IPropertyTypeCustomization
{
public:

	/**IPropertyTypeCustomization override*/
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FTimespanCustomization);
	}
	
private:

	/**Holds a handle to the property being edited.*/
	TSharedPtr<IPropertyHandle> PropertyHandle;

	/**Holds the text box for editing the Guid.*/
	TSharedPtr<SEditableTextBox> TextBox;

	/**Holds a flag indicating whether the current input is a valid GUID.*/
	bool InputValid = true;
	
	void HandleTimeChanges(const FText& NewText);
	FText HandleTimeValue() const;
	void HandleTextBoxTextCommited(const FText& NewText, ETextCommit::Type CommitInfo);
};
