// Copyright Epic Games, Inc. All Rights Reserved.

#include "ContentLib.h"

#define LOCTEXT_NAMESPACE "FContentLibModule"

DEFINE_LOG_CATEGORY(LogContentLib);

DEFINE_LOG_CATEGORY(LogContentLibAssetParsing);

void FContentLibModule::StartupModule() {}

void FContentLibModule::ShutdownModule() {}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FContentLibModule, ContentLib)