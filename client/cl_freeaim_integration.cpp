// ============================================================================
// FILE: src/game/client/cl_freeaim_integration.cpp
// DESC: Wire free-aim into Episodic client mode (ClientModeHLNormal).
//       Hooks CreateMove + OverrideView and defers to ClientModeShared.
// ============================================================================

#include "cbase.h"
#include "clientmode_hlnormal.h"   // your concrete clientmode class
#include "clientmode_shared.h"     // explicit base calls
#include "view_shared.h"
#include "vgui/ISurface.h"

// Free-aim entry points (from cl_freeaim_firstperson.cpp)
extern void FA_OnCreateMove(float dt, CUserCmd* cmd);
extern void FA_OnOverrideView(CViewSetup* pSetup);
extern Vector2D FA_GetOffsetDeg();

// NOTE: Add these to your clientmode_hlnormal.h inside the class definition:
//   virtual bool CreateMove( float flInputSampleTime, CUserCmd *cmd ) OVERRIDE;
//   virtual void OverrideView( CViewSetup *pSetup ) OVERRIDE;
//   virtual void PostRender() OVERRIDE;

// Inject free-aim, then call base behavior.
bool ClientModeHLNormal::CreateMove(float flInputSampleTime, CUserCmd* cmd)
{
    // Call free-aim first to modify the user command
    FA_OnCreateMove(flInputSampleTime, cmd);

    // Then let the base class handle the rest
    return ClientModeShared::CreateMove(flInputSampleTime, cmd);
}

// Apply free-aim camera first, then allow base to tweak FOV/HUD.
void ClientModeHLNormal::OverrideView(CViewSetup* pSetup)
{
    // Apply free-aim view modifications first
    FA_OnOverrideView(pSetup);

    // Then let base class handle other view modifications
    ClientModeShared::OverrideView(pSetup);
}

void ClientModeHLNormal::PostRender()
{
    // Call base first
    BaseClass::PostRender();
}