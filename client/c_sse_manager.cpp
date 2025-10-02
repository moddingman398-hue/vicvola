
#include "cbase.h"
#include "convar.h"

ConVar mb_pp_vignette_on_ads("mb_pp_vignette_on_ads", "1", FCVAR_ARCHIVE);

bool MB_GetWantsADS(); // from c_ads_helpers.cpp

void MB_DrawVignetteIfNeeded()
{
    if (!mb_pp_vignette_on_ads.GetBool()) return;
    if (!MB_GetWantsADS()) return;
    // No-op stub to keep builds clean across Mapbase variants.
}
