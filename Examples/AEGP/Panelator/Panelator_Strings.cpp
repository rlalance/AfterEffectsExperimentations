#include "Panelator.h"

typedef struct {
	unsigned long	index;
	char			str[256];
} TableString;

TableString		g_strs[StrID_NUMTYPES] = {
	StrID_NONE,							"",
	StrID_Name,							"Panelator IMGUI!",
	StrID_Description,					"Panelator, MIT",
	StrID_GenericError,					"Error enabling Panelator."
};


A_char	*GetStringPtr(int strNum)
{
	return g_strs[strNum].str;
}
