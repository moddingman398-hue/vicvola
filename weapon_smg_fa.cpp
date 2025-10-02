// src/game/shared/weapon_smg_fa.cpp
#include "cbase.h"
#include "weapon_smg_fa.h"

IMPLEMENT_NETWORKCLASS(CWeaponSMGFA, DT_WeaponSMGFA)

BEGIN_NETWORK_TABLE(CWeaponSMGFA, DT_WeaponSMGFA)
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeaponSMGFA)
END_PREDICTION_DATA()
#endif

#ifdef GAME_DLL
LINK_ENTITY_TO_CLASS(weapon_smg_fa, CWeaponSMGFA);

void CWeaponSMGFA::Precache()
{
    PrecacheScriptSound("Weapon_SMG1.Single");
    BaseClass::Precache();
}
#endif

void CWeaponSMGFA::PrimaryAttack()
{
#ifdef GAME_DLL
    if (!HasPrimaryAmmo() || gpGlobals->curtime < m_flNextPrimaryAttack || gpGlobals->curtime < m_flADSBlockUntil)
        return;

    if ((FireMode_t)m_iFireMode.Get() == FIREMODE_BURST && m_iBurstCount.Get() == 0)
        m_iBurstCount = 3;

    DoFireBullet(8.0f, 0.05f, 4);
#endif
}
