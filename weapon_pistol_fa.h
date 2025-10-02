// src/game/shared/weapon_pistol_fa.h
#pragma once
#include "weapon_base_fa.h"

class CWeaponPistolFA : public CBaseFAWeapon
{
public:
    DECLARE_CLASS(CWeaponPistolFA, CBaseFAWeapon);
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

#ifdef GAME_DLL
    void Precache() OVERRIDE;
#endif

    float GetFireRate() const OVERRIDE { return 0.30f; }
    bool  AllowsMode(FireMode_t m) const OVERRIDE { return m == FIREMODE_SEMI || m == FIREMODE_BURST; }
    float GetADSInDelay() const OVERRIDE { return 0.06f; }

    void PrimaryAttack() OVERRIDE;

#ifdef GAME_DLL
    // add acttable later if needed
#endif
};
