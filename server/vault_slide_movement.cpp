
#include "cbase.h"
#include "player.h"
#include "convar.h"
#include "in_buttons.h"

ConVar mb_vault_height_max("mb_vault_height_max", "48", FCVAR_ARCHIVE);
ConVar mb_slide_speed_min("mb_slide_speed_min", "220", FCVAR_ARCHIVE);

// Simple ledge vault: attempts to hop the player up a small step in front.
bool MB_TryVault(CBasePlayer* p) {
    if (!p) return false;
    trace_t tr;
    Vector fwd; p->EyeVectors(&fwd, NULL, NULL);
    Vector start = p->EyePosition();
    Vector end = start + fwd * 36.0f;

    UTIL_TraceHull(start, end,
        Vector(-16, -16, 0),
        Vector(16, 16, mb_vault_height_max.GetFloat()),
        MASK_PLAYERSOLID, p, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

    if (tr.DidHit() && tr.plane.normal.z > 0.5f)
    {
        Vector up = p->GetAbsOrigin();
        up.z += mb_vault_height_max.GetFloat();
        p->SetAbsOrigin(up);
        return true;
    }
    return false;
}

// Lightweight slide: only allowed while ducking and moving fast.
bool MB_TrySlide(CBasePlayer* p) {
    if (!p) return false;
    Vector vel = p->GetAbsVelocity();
    if (vel.Length2D() < mb_slide_speed_min.GetFloat())
        return false;

    if (!(p->m_nButtons & IN_DUCK))
        return false;

    Vector fwd; p->EyeVectors(&fwd, NULL, NULL);
    vel += fwd * 40.0f;
    p->SetAbsVelocity(vel);
    return true;
}
