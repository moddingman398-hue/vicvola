#include "cbase.h"
#include "ai_basenpc.h"
#include "props.h"
#include "physics_prop_ragdoll.h"
#include "bone_setup.h"
#include "ai_behavior.h"

//-----------------------------------------------------------------------------
// Static data definitions
//-----------------------------------------------------------------------------
CNPCDamageResponse::GibInfo_t CNPCDamageResponse::s_GibInfo[CNPCDamageResponse::GIB_MAX_TYPES] =
{
    { "ValveBiped.Bip01_Head1",
      { "models/gibs/hgibs.mdl", "models/gibs/hgibs_jaw.mdl", "models/gibs/hgibs_scapula.mdl" },
      5, 600.0f, 0.01f },
    { "ValveBiped.Bip01_L_UpperArm",
      { "models/gibs/antlion_gib_medium_1.mdl", "models/gibs/antlion_gib_medium_2.mdl", nullptr },
      3, 400.0f, 0.01f },
    { "ValveBiped.Bip01_R_UpperArm",
      { "models/gibs/antlion_gib_medium_1.mdl", "models/gibs/antlion_gib_medium_2.mdl", nullptr },
      3, 400.0f, 0.01f },
    { "ValveBiped.Bip01_L_Thigh",
      { "models/gibs/antlion_gib_large_1.mdl", "models/gibs/antlion_gib_large_2.mdl", nullptr },
      3, 350.0f, 0.01f },
    { "ValveBiped.Bip01_R_Thigh",
      { "models/gibs/antlion_gib_large_1.mdl", "models/gibs/antlion_gib_large_2.mdl", nullptr },
      3, 350.0f, 0.01f },
    { "ValveBiped.Bip01_Spine2",
      { "models/gibs/antlion_gib_large_3.mdl", "models/gibs/hgibs_rib.mdl", "models/gibs/hgibs_spine.mdl" },
      4, 300.0f, 0.5f }
};

//-----------------------------------------------------------------------------
// Gib type helper
//-----------------------------------------------------------------------------
CNPCDamageResponse::GibType_t CNPCDamageResponse::GetGibTypeFromHitgroup(int hitgroup)
{
    switch (hitgroup)
    {
    case HITGROUP_HEAD:     return GIB_HEAD;
    case HITGROUP_LEFTARM:  return GIB_LEFT_ARM;
    case HITGROUP_RIGHTARM: return GIB_RIGHT_ARM;
    case HITGROUP_LEFTLEG:  return GIB_LEFT_LEG;
    case HITGROUP_RIGHTLEG: return GIB_RIGHT_LEG;
    case HITGROUP_CHEST:
    case HITGROUP_STOMACH:  return GIB_TORSO;
    default:                return GIB_MAX_TYPES;
    }
}

//-----------------------------------------------------------------------------
// Gib creation
//-----------------------------------------------------------------------------
void CNPCDamageResponse::CreateGibs(CBaseAnimating* pVictim, GibType_t gibType, const Vector& damagePos, const Vector& damageForce)
{
    if (!pVictim || gibType >= GIB_MAX_TYPES)
        return;

    const GibInfo_t& info = s_GibInfo[gibType];
    for (int i = 0; i < info.gibCount; ++i)
    {
        const char* gibModel = info.gibModels[i % 3];
        if (!gibModel) continue;

        CBaseEntity* pGib = CreateEntityByName("prop_physics");
        if (pGib)
        {
            pGib->SetModel(gibModel);
            pGib->SetAbsOrigin(damagePos);
            pGib->SetAbsAngles(QAngle(0, 0, 0));
            pGib->Spawn();

            if (IPhysicsObject* pPhys = pGib->VPhysicsGetObject())
            {
                Vector vel = damageForce;
                vel.NormalizeInPlace();
                vel *= info.gibVelocity;
                pPhys->SetVelocity(&vel, nullptr);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Bone shrink (fallback -> bodygroup hiding)
//-----------------------------------------------------------------------------
void CNPCDamageResponse::ShrinkBone(CBaseAnimating* pVictim, const char* boneName, float scale)
{
    if (!pVictim || !boneName)
        return;

    int group = pVictim->FindBodygroupByName(boneName);
    if (group != -1)
    {
        pVictim->SetBodygroup(group, 1); // hide it
    }
}

//-----------------------------------------------------------------------------
// Gibbing entry point
//-----------------------------------------------------------------------------
void CNPCDamageResponse::ProcessGibbing(CAI_BaseNPC* pNPC, const CTakeDamageInfo& info)
{
    if (!pNPC) return;

    int hitgroup = info.GetDamageType(); // fallback if no trace
    GibType_t gibType = GetGibTypeFromHitgroup(hitgroup);

    Vector damagePos = info.GetDamagePosition();
    Vector damageForce = info.GetDamageForce();

    if (gibType != GIB_MAX_TYPES)
    {
        CreateGibs(pNPC, gibType, damagePos, damageForce);
        ShrinkBone(pNPC, s_GibInfo[gibType].boneName, s_GibInfo[gibType].boneScale);
    }
}

//-----------------------------------------------------------------------------
// Entry point: handle damage response
//-----------------------------------------------------------------------------
void CNPCDamageResponse::ProcessDamageResponse(CAI_BaseNPC* pNPC, const CTakeDamageInfo& info, trace_t* ptr)
{
    if (!pNPC || !pNPC->IsAlive())
        return;

    int hitgroup = HITGROUP_GENERIC;
    if (ptr && ptr->hitgroup > 0)
        hitgroup = ptr->hitgroup;

    float damagePercent = info.GetDamage() / MAX(1.0f, pNPC->GetMaxHealth());

    // Gibbing check
    if (pNPC->GetHealth() <= 0 || damagePercent > 0.5f)
        ProcessGibbing(pNPC, info);
}
