// src/game/shared/weapon_pistol_fa.cpp
#include "cbase.h"
#include "weapon_pistol_fa.h"

IMPLEMENT_NETWORKCLASS(CWeaponPistolFA, DT_WeaponPistolFA)

BEGIN_NETWORK_TABLE(CWeaponPistolFA, DT_WeaponPistolFA)
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeaponPistolFA)
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
LINK_ENTITY_TO_CLASS(weapon_pistol_fa, CWeaponPistolFA);

void CWeaponPistolFA::Precache()
{
    PrecacheScriptSound("Weapon_Pistol.Single");
    BaseClass::Precache();
}
#endif

void CWeaponPistolFA::PrimaryAttack()
{
#ifdef GAME_DLL
    if (!HasPrimaryAmmo() || gpGlobals->curtime < m_flNextPrimaryAttack || gpGlobals->curtime < m_flADSBlockUntil)
        return;

    if ((FireMode_t)m_iFireMode.Get() == FIREMODE_BURST && m_iBurstCount.Get() == 0)
        m_iBurstCount = 3;

    DoFireBullet(18.0f, 0.01f, 2);
#endif
}
