#include "AEConfig.h"
#include "AE_EffectVers.h"

#ifndef AE_OS_WIN
	#include "AE_General.r"
#endif

resource 'PiPL' (16000) {
	{	/* array properties: 12 elements */
		/* [1] */
		Kind {
			AEEffect
		},
		/* [2] */
		Name {
			"Custom_ECW_UI"
		},
		/* [3] */
		Category {
			"Sample Plug-ins"
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EffectMain"},
	#endif
#else
	#ifdef AE_OS_MAC
			CodeMacIntel64 {"EffectMain"},
	#endif
#endif
		
		/* [6] */
		AE_PiPL_Version {
			2,
			0
		},
		/* [7] */
		AE_Effect_Spec_Version {
			PF_PLUG_IN_VERSION,
			PF_PLUG_IN_SUBVERS
		},
		/* [8] */
		AE_Effect_Version {
			1048577	/* 2.0 */			
		},
		/* [9] */
		AE_Effect_Info_Flags {
			3						/* was 0 */
		},
		/* [10] */
		AE_Effect_Global_OutFlags {
			33588288
		},
		/* [11] */
		AE_Effect_Match_Name {
			"ADBE Custom_ECW_UI"
		},
		/* [12] */
		AE_Reserved_Info {
			0						/* was 0 */
		}
	}
};

