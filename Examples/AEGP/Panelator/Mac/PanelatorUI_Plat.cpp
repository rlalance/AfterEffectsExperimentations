#include <string>
#include <vector>
#include "PT_Err.h"
#include "PanelatorUI_Plat.h"
#include <limits>

#define kClickMePressed		'CLik'
#define	kMenuAboutToShow	kEventMenuPopulate


PanelatorUI_Plat::PanelatorUI_Plat(SPBasicSuite* spbP, AEGP_PanelH panelH, 
							AEGP_PlatformViewRef platformViewRef,
							AEGP_PanelFunctions1* outFunctionTable) : PanelatorUI( spbP, panelH, platformViewRef, outFunctionTable)
{


	
	//	shouldn't we cleanup and throw if we get an error??
}





	
void PanelatorUI_Plat::InvalidateAll() 
{

}
