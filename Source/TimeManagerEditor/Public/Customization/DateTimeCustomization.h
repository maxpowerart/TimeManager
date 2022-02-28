#pragma once
 
#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FDateTimeCustomization : public IPropertyTypeCustomization
{
public:

	/**IPropertyTypeCustomization override*/
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FDateTimeCustomization);
	}
	
private:

	/**Holds a handle to the property being edited.*/
	TSharedPtr<IPropertyHandle> PropertyHandle;

	/**Holds the date box for editing*/
	TSharedPtr<SEditableTextBox> DateBox;

	/**Holds the date box for editing*/
	TSharedPtr<SEditableTextBox> TimeBox;

	/**Holds a flag indicating whether the current input is a valid GUID.*/
	bool bInputTimeValid = true;

	/**Holds a flag indicating whether the current input is a valid GUID.*/
	bool bInputDateValid = true;
	
	void HandleTimeChanges(const FText& NewText);
	FText HandleTimeValue() const;
	void HandleTimeTextCommited(const FText& NewText, ETextCommit::Type CommitInfo);

	void HandleDateChanges(const FText& NewText);
	FText HandleDateValue() const;
	void HandleDateTextCommited(const FText& NewText, ETextCommit::Type CommitInfo);
};
