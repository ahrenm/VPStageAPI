// Copyright Epic Games, Inc. All Rights Reserved.

#include "VPStageAPIModule.h"

DEFINE_LOG_CATEGORY(StageAPI);

#define LOCTEXT_NAMESPACE "FVPStageAPIModule"

void FVPStageAPIModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FVPStageAPIModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVPStageAPIModule, VPStageAPI)