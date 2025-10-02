
#include "cbase.h"
#include "env_spike_impaler.h"
#include "convar.h"
#include "physics_prop_ragdoll.h"
ConVar mb_impale_required_speed("mb_impale_required_speed","180",FCVAR_ARCHIVE);
LINK_ENTITY_TO_CLASS(env_spike_impaler, CEnvSpikeImpaler);
BEGIN_DATADESC(CEnvSpikeImpaler) DEFINE_KEYFIELD(m_flRequiredSpeed, FIELD_FLOAT, "RequiredSpeed"), END_DATADESC()
void CEnvSpikeImpaler::Spawn(){ BaseClass::Spawn(); SetSolid(SOLID_BSP); SetMoveType(MOVETYPE_NONE);
    if(m_flRequiredSpeed<=0) m_flRequiredSpeed = mb_impale_required_speed.GetFloat(); }
void CEnvSpikeImpaler::Touch(CBaseEntity* other){
    CBaseAnimating* anim = dynamic_cast<CBaseAnimating*>(other); if(!anim||!anim->IsRagdoll()) return;
    Vector v; other->GetVelocity(&v,NULL); if(v.Length()<m_flRequiredSpeed) return;
    if(IPhysicsObject* phys = other->VPhysicsGetObject()) phys->EnableMotion(false);
}
