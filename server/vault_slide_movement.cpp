
#include "cbase.h"
#include "player.h"
#include "convar.h"
ConVar mb_vault_height_max("mb_vault_height_max","48",FCVAR_ARCHIVE);
ConVar mb_slide_speed_min("mb_slide_speed_min","220",FCVAR_ARCHIVE);
bool MB_TryVault(CBasePlayer* p){
    trace_t tr; Vector fwd; p->EyeVectors(&fwd,NULL,NULL);
    Vector start=p->EyePosition(), end=start+fwd*36.f;
    UTIL_TraceHull(start,end,Vector(-16,-16,0),Vector(16,16,mb_vault_height_max.GetFloat()),MASK_PLAYERSOLID,p,COLLISION_GROUP_PLAYER_MOVEMENT,&tr);
    if(tr.DidHit() && tr.plane.normal.z>0.5f){ p->SetAbsOrigin(p->GetAbsOrigin()+Vector(0,0,mb_vault_height_max.GetFloat())); return true; }
    return false;
}
bool MB_TrySlide(CBasePlayer* p){
    Vector vel=p->GetAbsVelocity();
    if(vel.Length2D()<mb_slide_speed_min.GetFloat()) return false;
    if(!(p->m_nButtons & IN_DUCK)) return false; p->SetFriction(0.1f); return true;
}
