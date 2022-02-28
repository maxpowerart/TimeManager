#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTimeManagerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}

private:
	bool OnPropertyChanged();
	void RegisterSettings();
	void UnregisterSettings();
};
