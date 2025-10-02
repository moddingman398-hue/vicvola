
#include "cbase.h"
#include "npc_gibbing_ext.h"
#include "gib.h"
#include "ai_basenpc.h"
#include "convar.h"

ConVar mb_helmet_pop_min_dmg("mb_helmet_pop_min_dmg", "30", FCVAR_ARCHIVE);
ConVar mb_head_gib_min_dmg("mb_head_gib_min_dmg", "60", FCVAR_ARCHIVE);
ConVar mb_stagger_leg_chance("mb_stagger_leg_chance", "1.0", FCVAR_ARCHIVE);

void MB_ApplyLimbDamageGibbing(CBaseEntity* victim, const CTakeDamageInfo& info, int hitgroup) {
    CAI_BaseNPC* npc = dynamic_cast<CAI_BaseNPC*>(victim);
    if (!npc) return;

    const float dmg = info.GetDamage();
    switch (hitgroup) {
    case HITGROUP_HEAD:
    {
        bool helmet = false; // TODO: wire your helmet/bodygroup check here
        if (!helmet && dmg >= mb_head_gib_min_dmg.GetFloat()) {
            npc->SetHealth(0);
            npc->Event_Killed(info);
            return;
        }
        npc->SetCondition(COND_LIGHT_DAMAGE);
    } break;

    case HITGROUP_LEFTLEG:
    case HITGROUP_RIGHTLEG:
    {
        if (RandomFloat(0.f, 1.f) <= mb_stagger_leg_chance.GetFloat())
            npc->SetCondition(COND_LIGHT_DAMAGE);
    } break;

    default: break;
    }
}
