#include "PanelatorUI.h"

#include <Carbon/Carbon.h>

class PanelatorUI_Plat : public PanelatorUI
{
public:
    explicit PanelatorUI_Plat(SPBasicSuite *spbP, AEGP_PanelH panelH, AEGP_PlatformViewRef platformWindowRef, AEGP_PanelFunctions1 *outFunctionTableP);
    virtual void InvalidateAll();
};

