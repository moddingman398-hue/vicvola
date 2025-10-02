// ============================================================================
// First-person free-aim core - MOVEMENT & CROSSHAIR FIX
// - FIXED: Never interferes with movement buttons (WASD, jump, crouch)
// - FIXED: Proper bullet direction calculation for crosshair alignment  
// - FIXED: Preserves all movement input while only modifying view angles
// ============================================================================
#include "cbase.h"
#include "c_baseplayer.h"
#include "iclientmode.h"
#include "input.h"
#include "clientmode_shared.h"
#include "view_shared.h"
#include "tier1/convar.h"
#include "in_buttons.h"
#include "vgui/ISurface.h"

// --- CVars via lookup (avoid extern link issues) ----------------------------
static ConVar* cv_m_yaw = nullptr, * cv_m_pitch = nullptr, * cv_sens = nullptr;
static inline float CVYaw() { if (!cv_m_yaw)   cv_m_yaw = cvar->FindVar("m_yaw");        return cv_m_yaw ? cv_m_yaw->GetFloat() : 0.022f; }
static inline float CVPitch() { if (!cv_m_pitch) cv_m_pitch = cvar->FindVar("m_pitch");      return cv_m_pitch ? cv_m_pitch->GetFloat() : 0.022f; }
static inline float CVSens() { if (!cv_sens)    cv_sens = cvar->FindVar("sensitivity");  return cv_sens ? cv_sens->GetFloat() : 3.0f; }

// Tunables
static ConVar fa_deadzone_deg("fa_deadzone_deg", "6.0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Dead-zone radius (deg).");
static ConVar fa_turn_speed("fa_turn_speed", "360.0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Bleed speed (deg/s) when outside dead-zone.");
static ConVar fa_recenterspeed("fa_recenterspeed", "0.0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Recenter speed (deg/s) when idle inside dead-zone. 0=off.");
static ConVar fa_invert_x("fa_invert_x", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Horizontal input multiplier (-1 to invert).");
static ConVar fa_enable("fa_enable", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL, "Enable free-aim system.");

// State
static QAngle   s_angView(0, 0, 0);     // camera
static QAngle   s_angShoot(0, 0, 0);    // bullets/crosshair
static Vector2D s_offset(0.0f, 0.0f);   // deg: +x yaw right, +y pitch up
static bool     s_inited = false;

// Store original movement inputs to preserve them
static int      s_originalButtons = 0;
static float    s_originalForwardMove = 0.0f;
static float    s_originalSideMove = 0.0f;
static float    s_originalUpMove = 0.0f;

// Helpers
static inline float ClampPitch(float p) { return clamp(p, -89.0f, 89.0f); }
static inline QAngle OffsetToAngles(const Vector2D& o) { return QAngle(o.y, o.x, 0.0f); }
static inline float NormAngle(float a) { while (a > 180) a -= 360; while (a < -180) a += 360; return a; }
static inline void ApproachAngle(float t, float& v, float s, float dt)
{
	const float d = NormAngle(t - v);
	const float step = s * dt;
	if (fabsf(d) <= step) v += d; else v += copysignf(step, d);
}

class CFreeAimSystem
{
public:
	void LevelInit()
	{
		s_inited = false;
		s_offset = Vector2D(0, 0);
	}

	void OnCreateMove(float dt, CUserCmd* cmd)
	{
		C_BasePlayer* pl = C_BasePlayer::GetLocalPlayer();
		if (!pl || !cmd) return;

		// CRITICAL FIX: Store original movement inputs BEFORE any processing
		s_originalButtons = cmd->buttons;
		s_originalForwardMove = cmd->forwardmove;
		s_originalSideMove = cmd->sidemove;
		s_originalUpMove = cmd->upmove;

		// Check if free-aim is enabled
		if (!fa_enable.GetBool())
		{
			// If disabled, just use normal view behavior
			if (!s_inited)
			{
				engine->GetViewAngles(s_angView);
				s_angShoot = s_angView;
				s_inited = true;
			}
			engine->GetViewAngles(s_angView);
			s_angShoot = s_angView;
			s_offset = Vector2D(0, 0);
			cmd->viewangles = s_angShoot;

			// CRITICAL: Restore movement inputs
			RestoreMovementInputs(cmd);
			return;
		}

		if (!s_inited)
		{
			engine->GetViewAngles(s_angView);
			s_angShoot = s_angView;
			s_inited = true;
		}

		// Check if player is aiming down sights
		bool bADS = false;
		// You can customize this condition - for example:
		// bADS = (cmd->buttons & IN_ATTACK2) != 0; // RMB to ADS

		// ADS: unify camera + aim; keep offset zeroed only while ADS is active
		if (bADS)
		{
			engine->GetViewAngles(s_angView);
			s_offset = Vector2D(0, 0);
			s_angShoot = s_angView;
			cmd->viewangles = s_angShoot;

			// CRITICAL: Restore movement inputs
			RestoreMovementInputs(cmd);
			return;
		}

		const float sens = CVSens();
		const float myaw = CVYaw();
		const float mpitch = CVPitch();

		// Process mouse input for free-aim
		const float invx = fa_invert_x.GetFloat();
		const float dx = float(cmd->mousedx) * sens * myaw * invx;
		const float dy = float(cmd->mousedy) * sens * mpitch;

		Vector2D off = s_offset;
		off.x -= dx; // subtract to match typical "mouse right turns view right" feel
		off.y += dy; // up is negative dy

		const float dead = Max(0.0f, fa_deadzone_deg.GetFloat());
		const float deadSq = dead * dead;

		// Clamp vertical offset to prevent extreme angles
		off.y = clamp(off.y, -dead * 1.5f, dead * 1.5f);

		const float turn = Max(0.0f, fa_turn_speed.GetFloat());
		const float rec = Max(0.0f, fa_recenterspeed.GetFloat());

		const float lenSq = off.x * off.x + off.y * off.y;
		if (lenSq > deadSq)
		{
			const float len = FastSqrt(lenSq);
			Vector2D dir = off * (1.0f / Max(len, 0.0001f));
			const float excess = len - dead;

			const QAngle bleed = OffsetToAngles(dir * excess);
			// Apply turn speed
			ApproachAngle(s_angView.x + bleed.x, s_angView.x, turn, dt);
			ApproachAngle(s_angView.y + bleed.y, s_angView.y, turn, dt);

			off = dir * dead; // clamp to rim; preserves feel at edge
		}
		else
		{
			// Only recenter if explicitly enabled AND no mouse input
			if (rec > 0.0f && cmd->mousedx == 0 && cmd->mousedy == 0)
			{
				ApproachAngle(0.0f, off.x, rec, dt);
				ApproachAngle(0.0f, off.y, rec, dt);
			}
		}

		s_offset = off;
		s_angShoot = s_angView + OffsetToAngles(s_offset);
		s_angShoot.x = ClampPitch(s_angShoot.x);

		// CRITICAL FIX: Set cmd->viewangles to shooting direction for bullet alignment
		cmd->viewangles = s_angShoot;

		// Set camera view angles separately  
		engine->SetViewAngles(s_angView);

		// CRITICAL: Always restore movement inputs after view processing
		RestoreMovementInputs(cmd);
	}

	void RestoreMovementInputs(CUserCmd* cmd)
	{
		// CRITICAL FIX: Restore ALL movement inputs to prevent interference
		cmd->buttons = s_originalButtons;
		cmd->forwardmove = s_originalForwardMove;
		cmd->sidemove = s_originalSideMove;
		cmd->upmove = s_originalUpMove;
	}

	void OnOverrideView(CViewSetup* setup)
	{
		C_BasePlayer* pl = C_BasePlayer::GetLocalPlayer();
		if (!pl || !setup) return;

		// Always use camera view for rendering, not shooting view
		setup->origin = pl->EyePosition();
		QAngle a = s_angView;
		a.x = ClampPitch(a.x);
		setup->angles = a;
	}
};

static CFreeAimSystem g_FreeAim;

// Exports for integration + HUD
void  FA_LevelInitPostEntity() { g_FreeAim.LevelInit(); }
void  FA_OnCreateMove(float dt, CUserCmd* cmd) { g_FreeAim.OnCreateMove(dt, cmd); }
void  FA_OnOverrideView(CViewSetup* p) { g_FreeAim.OnOverrideView(p); }

// Debug getters
Vector2D FA_GetOffsetDeg() { return s_offset; }
float    FA_GetDeadzoneDeg() { return fa_deadzone_deg.GetFloat(); }
QAngle   FA_GetShootAngles() { return s_angShoot; }

// AutoGameSystem to call LevelInitPostEntity without changing client mode
class CFreeAimInitSystem : public CAutoGameSystem
{
public:
	void LevelInitPostEntity() OVERRIDE { FA_LevelInitPostEntity(); }
} g_FreeAimInitSystem;