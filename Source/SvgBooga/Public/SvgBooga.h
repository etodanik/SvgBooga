#pragma once

#include "Modules/ModuleManager.h"

class FSvgBoogaModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
