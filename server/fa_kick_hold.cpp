// ============================================================================
// FILE: src/game/server/fa_kick_hold.cpp  (DROP-IN REPLACEMENT)
// DESC: Hold-to-charge kick (server). Pair with client fa_kick_input.cpp.
//       Bind: bind mouse3 "+fa_kick"
// ============================================================================
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "te_effect_dispatch.h"

#ifdef GAME_DLL

// ---- Tunables --------------------------------------------------------------
static ConVar fa_kick_range("fa_kick_range", "72", FCVAR_ARCHIVE, "Kick reach (units).");
static ConVar fa_kick_hull("fa_kick_hull", "16", FCVAR_ARCHIVE, "Kick hull half-size (units).");

static ConVar fa_kick_tmin("fa_kick_tmin", "0.10", FCVAR_ARCHIVE, "Minimum charge time (s).");
static ConVar fa_kick_tmax("fa_kick_tmax", "0.90", FCVAR_ARCHIVE, "Maximum charge time (s).");
static ConVar fa_kick_curve("fa_kick_curve", "1.25", FCVAR_ARCHIVE, "Charge curve exponent (1=linear).");

static ConVar fa_kick_dmg_min("fa_kick_dmg_min", "15", FCVAR_ARCHIVE, "Damage at min charge.");
static ConVar fa_kick_dmg_max("fa_kick_dmg_max", "55", FCVAR_ARCHIVE, "Damage at max charge.");

static ConVar fa_kick_force_min("fa_kick_force_min", "26000", FCVAR_ARCHIVE, "Impulse at min charge.");
static ConVar fa_kick_force_max("fa_kick_force_max", "115000", FCVAR_ARCHIVE, "Impulse at max charge.");
static ConVar fa_kick_upbias("fa_kick_upbias", "120", FCVAR_ARCHIVE, "Upward bias added to impulse.");

static ConVar fa_kick_cd_base("fa_kick_cd_base", "0.25", FCVAR_ARCHIVE, "Cooldown base (s).");
static ConVar fa_kick_cd_persec("fa_kick_cd_persec", "0.50", FCVAR_ARCHIVE, "Cooldown added per second of charge.");

static ConVar fa_kick_sound("fa_kick_sound", "Weapon_Crowbar.Single", FCVAR_NONE, "Kick sound script name.");

// ---- Per-player charge state ----------------------------------------------
static float g_flKickChargeStart[MAX_PLAYERS + 1] = { 0.0f };

static inline float Clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }

static void KickTrace(CBasePlayer* pl, trace_t& tr, Vector& forwardOut)
{
	AngleVectors(pl->EyeAngles(), &forwardOut);
	const Vector start = pl->Weapon_ShootPosition();
	const Vector end = start + forwardOut * fa_kick_range.GetFloat();

	const float h = fa_kick_hull.GetFloat();
	Vector mins(-h, -h, -h), maxs(h, h, h);

	UTIL_TraceHull(start, end, mins, maxs, MASK_SHOT_HULL, pl, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);
	if (tr.fraction == 1.0f || !tr.m_pEnt)
		UTIL_TraceLine(start, end, MASK_SHOT, pl, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);
}

// NOTE: trace_t MUST be non-const; DispatchTraceAttack expects trace_t*
static void ApplyKick(CBasePlayer* pl, CBaseEntity* hit, trace_t& tr, const Vector& fwd, float dmg, float impulse)
{
	if (!hit) return;

	// Damage (3-arg signature in SDK2013)
	CTakeDamageInfo info(pl, pl, dmg, DMG_CLUB | DMG_CRUSH);
	info.SetDamagePosition(tr.endpos);
	info.SetDamageForce(vec3_origin);
	hit->DispatchTraceAttack(info, fwd, &tr);
	ApplyMultiDamage();

	// Physics shove
	Vector imp = fwd * impulse;
	imp.z += fa_kick_upbias.GetFloat();

	if (IPhysicsObject* phys = hit->VPhysicsGetObject())
	{
		phys->ApplyForceOffset(imp, tr.endpos);
	}
	else
	{
		hit->SetAbsVelocity(hit->GetAbsVelocity() + imp * 0.01f); // cheap push
	}

	// Feedback
	pl->ViewPunch(QAngle(-2.5f, RandomFloat(-1.5f, 1.5f), 0.f));
	pl->EmitSound(fa_kick_sound.GetString());
}

// ---- Server commands -------------------------------------------------------

// Start charging
CON_COMMAND_F(fa_kick_start, "Begin charging kick", FCVAR_GAMEDLL)
{
	CBasePlayer* pl = UTIL_GetCommandClient();
	if (!pl || !pl->IsAlive()) return;

	g_flKickChargeStart[pl->entindex()] = gpGlobals->curtime;
}

// Release and execute kick
CON_COMMAND_F(fa_kick_release, "Release kick", FCVAR_GAMEDLL)
{
	CBasePlayer* pl = UTIL_GetCommandClient();
	if (!pl || !pl->IsAlive()) return;

	// Cooldown gate
	if (gpGlobals->curtime < pl->m_flNextAttack)
		return;

	const int idx = pl->entindex();
	const float now = gpGlobals->curtime;
	const float t0 = g_flKickChargeStart[idx] > 0.f ? g_flKickChargeStart[idx] : now;

	float charge = now - t0;
	const float tmin = fa_kick_tmin.GetFloat();
	const float tmax = fa_kick_tmax.GetFloat();

	float u = (charge - tmin) / Max(0.001f, (tmax - tmin));
	u = Clamp01(u);
	const float k = powf(u, Max(0.001f, fa_kick_curve.GetFloat()));

	const float dmg = Lerp(k, fa_kick_dmg_min.GetFloat(), fa_kick_dmg_max.GetFloat());
	const float imp = Lerp(k, fa_kick_force_min.GetFloat(), fa_kick_force_max.GetFloat());

	trace_t tr; Vector fwd;
	KickTrace(pl, tr, fwd);

	if (tr.fraction == 1.0f || !tr.m_pEnt)
	{
		pl->ViewPunch(QAngle(-1.5f, RandomFloat(-1.0f, 1.0f), 0.0f));
		pl->EmitSound(fa_kick_sound.GetString());
	}
	else
	{
		ApplyKick(pl, tr.m_pEnt, tr, fwd, dmg, imp);
	}

	// Cooldown scales with charge time
	pl->m_flNextAttack = now + fa_kick_cd_base.GetFloat() + charge * fa_kick_cd_persec.GetFloat();

	// reset
	g_flKickChargeStart[idx] = 0.f;
}

#endif // GAME_DLL
