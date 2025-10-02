// ============================================================================
// FILE: src/game/client/hud_crosshair_override.cpp (GUN ALIGNMENT FIX)
// DESC: Override the default crosshair to EXACTLY follow where bullets go
// ============================================================================
#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Panel.h"
#include "hud_crosshair.h"
#include "view.h"

using namespace vgui;

extern Vector2D FA_GetOffsetDeg();
extern QAngle FA_GetShootAngles();

static ConVar fa_crosshair_override("fa_crosshair_override", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Override crosshair to follow free-aim offset.");
static ConVar fa_crosshair_size("fa_crosshair_size", "6", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Free-aim crosshair size.");
static ConVar fa_crosshair_gap("fa_crosshair_gap", "3", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Free-aim crosshair gap.");
static ConVar fa_crosshair_thickness("fa_crosshair_thickness", "2", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Free-aim crosshair line thickness.");

int ScreenTransform(const Vector& point, Vector& screen);

class CHudCrosshairFA : public CHudElement, public Panel
{
    DECLARE_CLASS_SIMPLE(CHudCrosshairFA, Panel);
public:
    CHudCrosshairFA(const char* pElementName)
        : CHudElement(pElementName), Panel(g_pClientMode->GetViewport(), "HudCrosshairFA")
    {
        SetHiddenBits(HIDEHUD_PLAYERDEAD | HIDEHUD_CROSSHAIR);
        SetPaintBackgroundEnabled(false);

        // Make sure we cover the full screen
        SetSize(ScreenWidth(), ScreenHeight());
        SetPos(0, 0);
    }

    bool ShouldDraw() OVERRIDE
    {
        if (!fa_crosshair_override.GetBool())
            return false;

        C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
        if (!pPlayer)
            return false;

        // Don't draw if player is dead
        if (!pPlayer->IsAlive() && pPlayer->GetObserverMode() != OBS_MODE_IN_EYE)
            return false;

        return CHudElement::ShouldDraw();
    }

    void Paint() OVERRIDE
    {
        if (!ShouldDraw())
            return;

        C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
        if (!pPlayer)
            return;

        int sw, sh;
        surface()->GetScreenSize(sw, sh);

        // Method 1: Use actual bullet trace to determine crosshair position
        Vector vecStart = pPlayer->Weapon_ShootPosition();
        QAngle shootAngles = FA_GetShootAngles();
        Vector forward;
        AngleVectors(shootAngles, &forward);
        Vector vecEnd = vecStart + forward * 8192.0f; // Max trace distance

        // Trace where the bullet would actually go
        trace_t tr;
        UTIL_TraceLine(vecStart, vecEnd, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);

        // Project the hit point to screen coordinates
        Vector screen;
        bool bBehindCamera = (ScreenTransform(tr.endpos, screen) != 0);

        int crosshair_x, crosshair_y;

        if (!bBehindCamera)
        {
            // Convert normalized screen coordinates to pixel coordinates
            crosshair_x = (int)(0.5f * (1.0f + screen[0]) * sw);
            crosshair_y = (int)(0.5f * (1.0f - screen[1]) * sh); // Note: inverted Y
        }
        else
        {
            // Fallback to screen center if behind camera
            crosshair_x = sw / 2;
            crosshair_y = sh / 2;
        }

        const int size = fa_crosshair_size.GetInt();
        const int gap = fa_crosshair_gap.GetInt();
        const int thickness = fa_crosshair_thickness.GetInt();

        // Draw crosshair with thickness
        surface()->DrawSetColor(255, 255, 255, 255);

        for (int t = 0; t < thickness; t++)
        {
            int offset = t - thickness / 2;

            // Horizontal lines
            surface()->DrawLine(crosshair_x - size - gap, crosshair_y + offset, crosshair_x - gap, crosshair_y + offset);
            surface()->DrawLine(crosshair_x + gap, crosshair_y + offset, crosshair_x + size + gap, crosshair_y + offset);

            // Vertical lines
            surface()->DrawLine(crosshair_x + offset, crosshair_y - size - gap, crosshair_x + offset, crosshair_y - gap);
            surface()->DrawLine(crosshair_x + offset, crosshair_y + gap, crosshair_x + offset, crosshair_y + size + gap);
        }

        // Draw center dot
        surface()->DrawSetColor(255, 255, 255, 180);
        surface()->DrawFilledRect(crosshair_x - 1, crosshair_y - 1, crosshair_x + 2, crosshair_y + 2);

        // Debug: Draw where free-aim thinks it's pointing (red crosshair)
        Vector2D offDeg = FA_GetOffsetDeg();
        if (fabsf(offDeg.x) > 0.1f || fabsf(offDeg.y) > 0.1f)
        {
            surface()->DrawSetColor(255, 0, 0, 128); // Red debug crosshair
            int debug_x = sw / 2 + (int)(offDeg.x * 20); // Rough approximation
            int debug_y = sh / 2 - (int)(offDeg.y * 20);
            surface()->DrawLine(debug_x - 5, debug_y, debug_x + 5, debug_y);
            surface()->DrawLine(debug_x, debug_y - 5, debug_x, debug_y + 5);
        }
    }
};

DECLARE_HUDELEMENT(CHudCrosshairFA);