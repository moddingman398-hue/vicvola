// src/game/shared/weapon_smg_fa.h
#pragma once
#include "weapon_base_fa.h"

class CWeaponSMGFA : public CBaseFAWeapon
{
public:
    DECLARE_CLASS(CWeaponSMGFA, CBaseFAWeapon);
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

#ifdef GAME_DLL
    void Precache() OVERRIDE;
#endif

    float GetFireRate() const OVERRIDE { return 60.0f / 800.0f; } // ~800 RPM
    bool  AllowsMode(FireMode_t m) const OVERRIDE { return m == FIREMODE_AUTO || m == FIREMODE_BURST; }
    float GetADSInDelay() const OVERRIDE { return 0.05f; }

    void PrimaryAttack() OVERRIDE;

#ifdef GAME_DLL
    // add acttable later if needed
#endif
};
