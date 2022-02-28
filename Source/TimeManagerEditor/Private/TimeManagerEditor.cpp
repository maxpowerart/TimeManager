#include "TimeManagerEditor.h"

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "Customization/DateTimeCustomization.h"
#include "Customization/TimespanCustomization.h"
#include "TimeManager/Public/Types/TimeManagerProperties.h"

#define LOCTEXT_NAMESPACE "TimeManagerEditor"

void FTimeManagerEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();
	
	/**Modify FTimespan property editor*/
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout("Timespan");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("Timespan",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTimespanCustomization::MakeInstance));

	PropertyEditorModule.UnregisterCustomPropertyTypeLayout("DateTime");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout("DateTime",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDateTimeCustomization::MakeInstance));
	
	PropertyEditorModule.NotifyCustomizationModuleChanged();
	
	/**Register plugin settings in ProjectSettings*/
	RegisterSettings();
}

void FTimeManagerEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout("Timespan");
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout("DateTime");
	
	UnregisterSettings();
}

bool FTimeManagerEditorModule::OnPropertyChanged()
{
	UTimeManagerProperties* Settings = GetMutableDefault<UTimeManagerProperties>();
	if(IsValid(Settings))
	{
		Settings->SaveConfig();
	}
	return true;
}

void FTimeManagerEditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Time Manager",
			LOCTEXT("RuntimeSettingsName", "Time Manager"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Time Manager plugin"),
			GetMutableDefault<UTimeManagerProperties>());

		if(SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FTimeManagerEditorModule::OnPropertyChanged);
		}
	}
}

void FTimeManagerEditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Time Manager");
	}
}
IMPLEMENT_MODULE(FTimeManagerEditorModule, TimeManagerEditor);
