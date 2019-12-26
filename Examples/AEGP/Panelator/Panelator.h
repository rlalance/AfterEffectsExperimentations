#include "AEConfig.h"
#ifdef AE_OS_WIN
	#include <windows.h>
#endif

#include "entry.h"
#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#ifndef DEBUG
	#define DEBUG
#else
	#error
#endif
#include "SuiteHelper.h"
#include "String_Utils.h"
#include "AE_EffectSuites.h"
#include "PanelatorUI_Plat.h"
#include "PT_Err.h"

typedef enum {
	StrID_NONE,							
	StrID_Name,							
	StrID_Description,
	StrID_GenericError,
	StrID_NUMTYPES
} StrIDType;

// This entry point is exported through the PiPL (.r file)
extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunc;
