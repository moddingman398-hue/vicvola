// src/game/shared/weapon_base_fa.cpp
#include "cbase.h"
#include "weapon_base_fa.h"
#include "in_buttons.h"
#ifdef GAME_DLL
#include "te_effect_dispatch.h"
#endif

IMPLEMENT_NETWORKCLASS(CBaseFAWeapon, DT_BaseFAWeapon)

BEGIN_NETWORK_TABLE(CBaseFAWeapon, DT_BaseFAWeapon)
#ifdef CLIENT_DLL
RecvPropBool(RECVINFO(m_bADS)),
RecvPropInt(RECVINFO(m_iFireMode)),
RecvPropInt(RECVINFO(m_iBurstCount)),
#else
SendPropBool(SENDINFO(m_bADS)),
SendPropInt(SENDINFO(m_iFireMode), 3, SPROP_UNSIGNED),
SendPropInt(SENDINFO(m_iBurstCount), 8, SPROP_UNSIGNED),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CBaseFAWeapon)
DEFINE_PRED_FIELD(m_bADS, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_iFireMode, FIELD_INTEGER, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_iBurstCount, FIELD_INTEGER, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif

CBaseFAWeapon::CBaseFAWeapon()
{
    m_bADS = false;
    m_iFireMode = FIREMODE_SEMI;
    m_iBurstCount = 0;
    m_flNextBurstShot = 0.0f;
    m_flADSBlockUntil = 0.0f;
}

void CBaseFAWeapon::StartADS(bool bIn)
{
    m_bADS = bIn;
    m_flADSBlockUntil = gpGlobals->curtime + (bIn ? GetADSInDelay() : GetADSOutDelay());
}

void CBaseFAWeapon::CycleFireMode()
{
    int next = (m_iFireMode.Get() + 1) % 3;
    for (int i = 0; i < 3; ++i)
    {
        FireMode_t tryMode = (FireMode_t)((next + i) % 3);
        if (AllowsMode(tryMode))
        {
            m_iFireMode = (int)tryMode;
            break;
        }
    }
#ifdef GAME_DLL
    EmitSound("Weapon.FireMode");
#endif
}

void CBaseFAWeapon::ItemPostFrame()
{
    CBasePlayer* p = ToBasePlayer(GetOwner());
    if (!p) { BaseClass::ItemPostFrame(); return; }

    // ADS hold on RMB
    if (p->m_nButtons & IN_ATTACK2)
    {
        if (!m_bADS.Get()) StartADS(true);
    }
    else if (m_bADS.Get())
    {
        StartADS(false);
    }

    // Cycle fire mode on reload+use
    if ((p->m_nButtons & IN_RELOAD) && (p->m_nButtons & IN_USE))
    {
        CycleFireMode();
    }

    HandleBurstFire();
    BaseClass::ItemPostFrame();
}

void CBaseFAWeapon::HandleBurstFire()
{
#ifdef GAME_DLL
    if ((FireMode_t)m_iFireMode.Get() == FIREMODE_BURST &&
        m_iBurstCount.Get() > 0 &&
        gpGlobals->curtime >= m_flNextBurstShot)
    {
        PrimaryAttack();
        m_iBurstCount = m_iBurstCount.Get() - 1;
        m_flNextBurstShot = gpGlobals->curtime + 0.06f;
    }
#endif
}

#ifdef GAME_DLL
void CBaseFAWeapon::DoFireBullet(float damage, float spread, int tracerFreq)
{
    CBasePlayer* pOwner = ToBasePlayer(GetOwner());
    if (!pOwner) return;

    Vector vecSrc = pOwner->Weapon_ShootPosition();
    QAngle angAiming = pOwner->EyeAngles();

    FireBulletsInfo_t info;
    info.m_iShots = 1;
    info.m_vecSrc = vecSrc;
    AngleVectors(angAiming, &info.m_vecDirShooting);
    info.m_vecSpread = Vector(spread, spread, 0);
    info.m_flDistance = MAX_TRACE_LENGTH;
    info.m_iTracerFreq = tracerFreq;
    info.m_flDamage = damage;
    info.m_iAmmoType = GetPrimaryAmmoType();

    pOwner->FireBullets(info);
    WeaponSound(SINGLE);
    pOwner->ViewPunch(QAngle(-0.25f, RandomFloat(-0.25f, 0.25f), 0));
    SendWeaponAnim(GetPrimaryAttackActivity());
    m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
    m_iClip1 = m_iClip1.Get() - 1;
    if (m_iClip1.Get() <= 0 && UsesClipsForAmmo1())
        Reload();
}
#endif
