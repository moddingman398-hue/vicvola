
#include "cbase.h"
#include "improv_melee_pickup.h"
#include "basecombatweapon.h"
#include "props.h"
bool MB_TryPickupPropAsMelee(CBasePlayer* pl, CBaseEntity* ent){
    CPhysicsProp* prop = dynamic_cast<CPhysicsProp*>(ent);
    if(!prop || !prop->VPhysicsGetObject()) return false;
    CBaseCombatWeapon* w = pl->Weapon_OwnsThisType("weapon_melee_generic") ? pl->Weapon_GetWeaponByType("weapon_melee_generic") : CreateNoSpawn<CBaseCombatWeapon>("weapon_melee_generic");
    if(!w) return false;
    if(!pl->Weapon_OwnsThisType("weapon_melee_generic")){ w->Spawn(); pl->Weapon_Equip(w); }
    float mass=0.f; prop->VPhysicsGetObject()->GetMass(&mass);
    w->SetCustomDamage( clamp(mass*0.4f, 10.f, 80.f) ); UTIL_Remove(prop); return true;
}
