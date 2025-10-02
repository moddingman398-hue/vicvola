
// [VicVolA] c_ads_helpers.cpp — ADS fraction + FOV lerp
#include "cbase.h"
#include "convar.h"

ConVar mb_ads_speed("mb_ads_speed","10.0", FCVAR_ARCHIVE, "ADS lerp speed (higher=faster).");
ConVar mb_ads_fov("mb_ads_fov","54", FCVAR_ARCHIVE, "Target FOV when aiming.");
static float s_ADSFrac = 0.0f;
static bool  s_WantsADS = false;

void MB_SetWantsADS(bool b) { s_WantsADS = b; }
bool MB_GetWantsADS()       { return s_WantsADS; }

void MB_UpdateADS(float frametime)
{
    const float target = s_WantsADS ? 1.0f : 0.0f;
    const float speed  = clamp(mb_ads_speed.GetFloat(), 1.f, 60.f);
    s_ADSFrac += (target - s_ADSFrac) * clamp(frametime * speed, 0.f, 1.f);
}

float MB_ComputeADSFOV(float baseFov)
{
    // Lerp base to ADS FOV
    return Lerp(s_ADSFrac, baseFov, mb_ads_fov.GetFloat());
}

// Console helpers for testing
CON_COMMAND(mb_ads_on,  "Force ADS on")  { MB_SetWantsADS(true);  }
CON_COMMAND(mb_ads_off, "Force ADS off") { MB_SetWantsADS(false); }
