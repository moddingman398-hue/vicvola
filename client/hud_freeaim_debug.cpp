// ============================================================================
// Free-aim debug HUD: dead-zone ring + offset reticle
// - Looks up default_fov at runtime; falls back to 90 if missing
// - Shows crosshair offset to indicate where bullets will go
// ============================================================================
#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"
#include "vgui_controls/Panel.h"

using namespace vgui;

extern Vector2D FA_GetOffsetDeg();
extern float    FA_GetDeadzoneDeg();

static ConVar fa_dbg("fa_dbg", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Draw free-aim debug HUD.");

static ConVar* cv_default_fov = nullptr;
static inline float CVDefaultFOV()
{
	if (!cv_default_fov) cv_default_fov = cvar->FindVar("default_fov");
	return cv_default_fov ? cv_default_fov->GetFloat() : 90.0f;
}

class CHudFreeAimDebug : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE(CHudFreeAimDebug, Panel);
public:
	CHudFreeAimDebug(const char* pElementName)
		: CHudElement(pElementName), Panel(g_pClientMode->GetViewport(), "HudFreeAimDebug")
	{
		SetHiddenBits(0);
	}

	void Paint() OVERRIDE
	{
		if (!fa_dbg.GetBool()) return;

		int sw, sh; surface()->GetScreenSize(sw, sh);
		const float fov = CVDefaultFOV();
		const float pxPerDegX = sw / Max(fov, 1.0f);
		const float pxPerDegY = sh / Max(fov * (sh / (float)sw), 1.0f);

		const Vector2D offDeg = FA_GetOffsetDeg();
		const float dead = FA_GetDeadzoneDeg();

		const int cx = sw / 2, cy = sh / 2;
		const int rx = (int)(dead * pxPerDegX);
		const int ry = (int)(dead * pxPerDegY);

		// Draw deadzone circle
		surface()->DrawSetColor(255, 255, 255, 60);
		for (int i = 0; i < 360; i += 6)
		{
			float a0 = DEG2RAD((float)i), a1 = DEG2RAD((float)(i + 6));
			int x0 = cx + (int)(cosf(a0) * rx), y0 = cy + (int)(sinf(a0) * ry);
			int x1 = cx + (int)(cosf(a1) * rx), y1 = cy + (int)(sinf(a1) * ry);
			surface()->DrawLine(x0, y0, x1, y1);
		}

		// Draw offset crosshair (this shows where bullets will go)
		const int rxp = cx + (int)(offDeg.x * pxPerDegX);
		const int ryp = cy - (int)(offDeg.y * pxPerDegY);

		surface()->DrawSetColor(255, 255, 255, 180);
		surface()->DrawLine(rxp - 6, ryp, rxp + 6, ryp);
		surface()->DrawLine(rxp, ryp - 6, rxp, ryp + 6);

		// Draw center crosshair (camera center)
		surface()->DrawSetColor(255, 0, 0, 100);
		surface()->DrawLine(cx - 3, cy, cx + 3, cy);
		surface()->DrawLine(cx, cy - 3, cx, cy + 3);
	}
};

DECLARE_HUDELEMENT(CHudFreeAimDebug);