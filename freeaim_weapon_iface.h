// ============================================================================
// FILE: src/game/shared/freeaim_weapon_iface.h
// ============================================================================
#pragma once
#include "cbase.h"

enum FireMode_t : int { FIREMODE_SEMI = 0, FIREMODE_BURST, FIREMODE_AUTO };

class IFreeAimWeapon
{
public:
	virtual bool IsAimingDownSights() const = 0;
	virtual FireMode_t GetFireMode() const = 0;
	virtual void CycleFireMode() = 0;
	virtual int  GetBurstCount() const = 0;
};

inline IFreeAimWeapon* FA_GetIFace( CBaseCombatWeapon *w )
{
	return w ? dynamic_cast<IFreeAimWeapon*>(w) : nullptr;
}

#ifdef CLIENT_DLL
inline bool FreeAim_IsADS()
{
	C_BasePlayer *pl = C_BasePlayer::GetLocalPlayer();
	if ( !pl ) return false;
	CBaseCombatWeapon *w = pl->GetActiveWeapon();
	if ( !w ) return false;
	if ( IFreeAimWeapon *ifa = FA_GetIFace( w ) )
		return ifa->IsAimingDownSights();
	return false;
}
#else
inline bool FreeAim_IsADS() { return false; }
#endif
