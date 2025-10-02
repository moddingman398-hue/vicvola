// src/game/shared/weapon_base_fa.h
#pragma once
#include "basehlcombatweapon_shared.h"
#include "freeaim_weapon_iface.h"

class CBaseFAWeapon : public CBaseHLCombatWeapon, public IFreeAimWeapon
{
public:
    DECLARE_CLASS(CBaseFAWeapon, CBaseHLCombatWeapon);
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();

    CBaseFAWeapon();

    // IFreeAimWeapon
    bool        IsAimingDownSights() const OVERRIDE { return m_bADS.Get(); }
    FireMode_t  GetFireMode() const OVERRIDE { return (FireMode_t)m_iFireMode.Get(); }
    void        CycleFireMode() OVERRIDE;
    int         GetBurstCount() const OVERRIDE { return m_iBurstCount.Get(); }

    // CBaseCombatWeapon
    void ItemPostFrame() OVERRIDE;

    // Tuning hooks
    virtual float GetADSInDelay()  const { return 0.08f; }
    virtual float GetADSOutDelay() const { return 0.08f; }
    virtual bool  AllowsMode(FireMode_t) const { return true; }
    virtual float GetFireRate() const { return 0.2f; } // fallback

protected:
    void StartADS(bool bIn);
    void HandleBurstFire();

#ifdef GAME_DLL
    void DoFireBullet(float damage, float spread, int tracerFreq = 0);
#endif

    CNetworkVar(bool, m_bADS);
    CNetworkVar(int, m_iFireMode);
    CNetworkVar(int, m_iBurstCount);
    float m_flNextBurstShot;
    float m_flADSBlockUntil; // gate shooting during ADS-in/out
};
