
#include "cbase.h"
#include "improv_melee_pickup.h"
#include "basecombatweapon.h"
#include "props.h"
#include "entitylist.h"

bool MB_TryPickupPropAsMelee(CBasePlayer* pl, CBaseEntity* ent) {
    if (!pl || !ent) return false;

    CPhysicsProp* prop = dynamic_cast<CPhysicsProp*>(ent);
    if (!prop || !prop->VPhysicsGetObject()) return false;

    CBaseCombatWeapon* wpn = pl->Weapon_OwnsThisType("weapon_melee_generic");
    if (!wpn)
    {
        wpn = dynamic_cast<CBaseCombatWeapon*>(CreateEntityByName("weapon_melee_generic"));
        if (!wpn) return false;

        DispatchSpawn(wpn);
        wpn->SetAbsOrigin(pl->GetAbsOrigin());
        wpn->SetOwnerEntity(pl);
        pl->Weapon_Equip(wpn);
    }

    float mass = 0.0f;
    if (IPhysicsObject* phys = prop->VPhysicsGetObject())
        mass = phys->GetMass();
    int heft = clamp((int)(mass * 0.4f), 10, 80);
    wpn->SetSubType(heft); // weapon_melee_generic can read this and scale damage.

    UTIL_Remove(prop);
    return true;
}
