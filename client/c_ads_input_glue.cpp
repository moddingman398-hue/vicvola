
// [VicVolA] c_ads_input_glue.cpp — poll RMB and set ADS each frame
#include "cbase.h"
#include "tier1/convar.h"
#include "inputsystem/iinputsystem.h"
#include "igameevents.h"
#include "c_ads_helpers.h"

// Exposed by the engine
extern IInputSystem *g_pInputSystem;

ConVar mb_ads_enable("mb_ads_enable","1", FCVAR_ARCHIVE, "Master switch for ADS.");
ConVar mb_ads_bind_mode("mb_ads_bind_mode","1", FCVAR_ARCHIVE, "1=Use RMB by polling input system, 0=manual (use mb_ads_on/off).");
ConVar mb_ads_debug("mb_ads_debug","0", FCVAR_ARCHIVE, "Spam debug prints for ADS.");

class CADSInputGlue : public CAutoGameSystemPerFrame {
public:
    const char *Name() override { return "ADSInputGlue"; }
    void Update( float frametime ) override
    {
        if ( !mb_ads_enable.GetBool() ) return;

        bool wants = MB_GetWantsADS();

        if ( mb_ads_bind_mode.GetInt() == 1 )
        {
            // Poll RMB state directly from input system (client-side only)
            bool rmb = ( g_pInputSystem && g_pInputSystem->IsButtonDown( MOUSE_RIGHT ) );
            wants = rmb;
        }

        MB_SetWantsADS( wants );

        if ( mb_ads_debug.GetBool() )
            Msg("[ADS] wants=%d\n", wants ? 1:0 );
    }
} g_ADSInputGlue;
